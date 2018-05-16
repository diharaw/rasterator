#include <application.hpp>
#include <model.hpp>
#include <math/vec3.hpp>
#include <rasterator.hpp>
#include <math/transform.hpp>
#include <math/utility.hpp>

#include <iostream>
#include <climits>
#include <algorithm>

//#define OLD_BARYCENTRIC

class Texture
{
public:
	uint32_t* m_Pixels;
	float*	  m_Depth;
	uint32_t  m_Width;
	uint32_t  m_Height;

public:
	Texture(uint32_t width, uint32_t height, bool depth = false) : m_Width(width), m_Height(height)
	{
		if (depth)
		{
			m_Pixels = nullptr;
			m_Depth = new float[width * height];
		}
		else
		{
			m_Pixels = new uint32_t[width * height];;
			m_Depth = nullptr;
		}
	}

	~Texture()
	{
		RST_SAFE_DELETE_ARRAY(m_Pixels);
		RST_SAFE_DELETE_ARRAY(m_Depth);
	}

	void SetDepth(float depth, uint32_t x, uint32_t y)
	{
		y = m_Height - y - 1;

		if (m_Depth)
		{
			if (x > m_Width - 1 || x < 0)
				return;

			if (y > m_Height - 1 || y < 0)
				return;

			m_Depth[y * m_Width + x] = depth;
		}
	}

	void SetColor(uint32_t color, uint32_t x, uint32_t y)
	{
		y = m_Height - y - 1;

		if (m_Pixels)
		{
			if (x > m_Width - 1 || x < 0)
				return;

			if (y > m_Height - 1 || y < 0)
				return;

			m_Pixels[y * m_Width + x] = color;
		}
	}

	void Clear()
	{
		if (m_Depth)
		{
			uint32_t size = m_Width * m_Height;
			float depth = INFINITY;

			for (uint32_t i = 0; i < size; i++)
				m_Depth[i] = depth;
		}
	}

	void Clear(float r, float g, float b, float a)
	{
		if (m_Pixels)
		{
			uint32_t color = RST_COLOR_ARGB(r, g, b, a);
			uint32_t size = m_Width * m_Height;

			for (uint32_t i = 0; i < size; i++)
				m_Pixels[i] = color;
		}
	}
};

class Color
{
public:
	float R, G, B, A;

	Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}

	Color operator + (const Color &c) const
	{
		return Color(R + c.R, G + c.G, B + c.B, A + c.A);
	}

	Color operator - (const Color &c) const
	{
		return Color(R - c.R, G - c.G, B - c.B, A - c.A);
	}

	Color operator * (float f) const
	{
		return Color(R * f, G * f, B * f, A * f);
	}
};

class Demo : public Application
{
	Texture* m_Color;
	Texture* m_Depth;

	Model* model;
	Color white = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mat4f m_view;
	mat4f m_projection;
	mat4f m_model;
	mat4f m_vp;
	mat4f m_mvp;

private:
#ifdef OLD_BARYCENTRIC
	vec3f barycentric(vec3f A, vec3f B, vec3f C, vec3f P) 
	{
		vec3f s[2];
		for (int i = 2; i--; ) {
			s[i][0] = C[i] - A[i];
			s[i][1] = B[i] - A[i];
			s[i][2] = A[i] - P[i];
		}
		vec3f u = s[0].cross(s[1]);
		if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		{
			vec3f r = vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
			return r;
		}
		return vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
	}
#else
	float edge_function(const vec3f &a, const vec3f &b, const vec3f &c)
	{
		return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
	}

	float edge_function(const vec2f &a, const vec2f &b, const vec2f &c)
	{
		return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
	}

	vec3f barycentric(vec3f v0, vec3f v1, vec3f v2, vec3f p)
	{
		float w0 = edge_function(v1, v2, p);
		float w1 = edge_function(v2, v0, p);
		float w2 = edge_function(v0, v1, p);

		return vec3f(w0, w1, w2);
	}

	vec3f barycentric(vec2f v0, vec2f v1, vec2f v2, vec2f p)
	{
		float w0 = edge_function(v1, v2, p);
		float w1 = edge_function(v2, v0, p);
		float w2 = edge_function(v0, v1, p);

		return vec3f(w0, w1, w2);
	}
#endif

	void triangle(vec3f *pts, Texture* colorTex, Texture* depthTex, uint32_t color)
	{
		vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
		vec2f clamp(colorTex->m_Width - 1, colorTex->m_Height - 1);

		for (int i = 0; i<3; i++)
		{
			for (int j = 0; j<2; j++)
			{
				bboxmin[j] = std::max(0.0f, std::min(bboxmin[j], pts[i][j]));
				bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
			}
		}

		vec3f P;

		float area = edge_function(pts[0], pts[1], pts[2]);

		for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
		{
			for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
			{
				vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
				if (bc_screen[0] >= 0 && bc_screen[1] >= 0 && bc_screen[2] >= 0)
				{
					bc_screen[0] /= area;
					bc_screen[1] /= area;
					bc_screen[2] /= area;

					float z = pts[0].z * bc_screen[0] + pts[1].z * bc_screen[1] + pts[2].z * bc_screen[2];
					//float z = 1 / oneOverZ;

					if (z < depthTex->m_Depth[int(P.x + P.y * depthTex->m_Width)])
					{
						depthTex->m_Depth[int(P.x + P.y * depthTex->m_Width)] = z;
						colorTex->SetColor(color, P.x, P.y);
					}
				}
			}
		}
	}

	inline vec2f convert_to_screen_space(const float& x, const float& y, const uint32_t& width, const uint32_t& height)
	{
		return vec2f(int((((x + 1) * width) * 0.5f) + 0.5f), int((((y + 1) * height) * 0.5f) + 0.5f));
	}

	inline void triangle(const vec3f& v0, const vec3f& v1, const vec3f& v2, const mat4f& mvp, Texture* color_tex, Texture* depth_tex)
	{
		// Convert to screen space
		vec4f v0ndc = mvp * vec4f(v0.x, v0.y, v0.z, 1.0f);
		vec4f v1ndc = mvp * vec4f(v1.x, v1.y, v1.z, 1.0f);
		vec4f v2ndc = mvp * vec4f(v2.x, v2.y, v2.z, 1.0f);

		// Perspective division
		v0ndc = v0ndc / v0ndc.w;
		v1ndc = v1ndc / v1ndc.w;
		v2ndc = v2ndc / v2ndc.w;

		// Screen coords
		vec2f v0screen = convert_to_screen_space(v0ndc.x, v0ndc.y, m_width, m_height);
		vec2f v1screen = convert_to_screen_space(v1ndc.x, v1ndc.y, m_width, m_height);
		vec2f v2screen = convert_to_screen_space(v2ndc.x, v2ndc.y, m_width, m_height);

		// Find triangle bounding box
		vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

		vec2f clamp(m_width - 1, m_height - 1);

		// Min
		bboxmin.x = std::max(0.0f, std::min(bboxmin.x, v0screen.x));
		bboxmin.x = std::max(0.0f, std::min(bboxmin.x, v1screen.x));
		bboxmin.x = std::max(0.0f, std::min(bboxmin.x, v2screen.x));

		bboxmin.y = std::max(0.0f, std::min(bboxmin.y, v0screen.y));
		bboxmin.y = std::max(0.0f, std::min(bboxmin.y, v1screen.y));
		bboxmin.y = std::max(0.0f, std::min(bboxmin.y, v2screen.y));

		// Max
		bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, v0screen.x));
		bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, v1screen.x));
		bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, v2screen.x));
						   
		bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, v0screen.y));
		bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, v1screen.y));
		bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, v2screen.y));

		// Triangle area
		float area = edge_function(v0screen, v1screen, v2screen);

		vec2f p;

		for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
		{
			for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
			{
				// Calculate barycentric coordinates
				vec3f bcoords = barycentric(v0screen, v1screen, v2screen, p);

				if (bcoords.x >= 0 && bcoords.y >= 0 && bcoords.z >= 0)
				{
					bcoords.x /= area;
					bcoords.y /= area;
					bcoords.z /= area;

					float z = v0ndc.z * bcoords.x + v1ndc.z * bcoords.y + v2ndc.z * bcoords.z;

					if (z < depth_tex->m_Depth[int(p.x + p.y * depth_tex->m_Width)])
					{
						depth_tex->m_Depth[int(p.x + p.y * depth_tex->m_Width)] = z;
						color_tex->SetColor(RST_COLOR_RGBA(1.0f, 1.0f, 1.0f, 1.0f), p.x, p.y);
					}
				}
			}
		}
	}

	vec3f world2screen(vec3f v, int width, int height) 
	{
		return vec3f(int((v.x + 1.)*width / 2. + .5), int((v.y + 1.)*height / 2. + .5), v.z);
	}

protected:
	bool initialize() override
	{
		model = new Model("african_head.obj");

		m_Color = new Texture(m_width, m_height);
		m_Depth = new Texture(m_width, m_height, true);

		m_view = lookat(vec3f(0.0f, 0.0f, 2.0f), vec3f(0.0f), vec3f(0.0f, 1.0f, 0.0f));
		m_projection = perspective(float(m_width) / float(m_height), radians(60.0f), 0.1f, 100.0f);
		m_vp = m_projection * m_view;

		return true;
	}

	void frame() override
	{
		m_Depth->Clear();
		m_Color->Clear(0.0f, 0.0f, 0.0f, 1.0f);

		vec3f light_dir = vec3f(0.0f, 0.0f, -1.0f);
		light_dir.normalize();

		m_model = rotation(radians(SDL_GetTicks() * 0.05f), vec3f(0.0f, 1.0f, 0.0f));
		m_mvp = m_vp * m_model;

		#pragma omp parallel for
		for (int i = 0; i<model->nfaces(); i++) 
		{
			std::vector<int>& face = model->face(i);
			triangle(model->vert(face[0]), model->vert(face[1]), model->vert(face[2]), m_mvp, m_Color, m_Depth);
		}

		update_backbuffer(m_Color->m_Pixels);
	}

	void shutdown() override
	{
		RST_SAFE_DELETE(model);
		RST_SAFE_DELETE(m_Depth);
		RST_SAFE_DELETE(m_Color);
	}
};

RST_DECLARE_MAIN(Demo);