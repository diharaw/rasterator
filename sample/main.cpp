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

	vec3f barycentric(vec3f v0, vec3f v1, vec3f v2, vec3f p)
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

			vec3f screen_coords[3];
			vec3f world_coords[3];

			for (int j = 0; j<3; j++) 
			{
				vec3f v = model->vert(face[j]);
				vec4f sv = m_mvp * vec4f(v.x, v.y, v.z, 1.0f);
				float depth = sv.z;
				sv = sv / sv.w;

				// Convert to [0 - 1] range
				screen_coords[j] = ((vec3f(sv.x, sv.y, sv.z) + vec3f(1.0f)) / 2.0f);

				// Convert to device coordinates
				screen_coords[j].x = int(screen_coords[j].x * (m_Color->m_Width - 1.0f) + 0.5f);
				screen_coords[j].y = int(screen_coords[j].y * (m_Color->m_Height - 1.0f) + 0.5f);
				screen_coords[j].z = depth;

				vec4f w = m_model * vec4f(v.x, v.y, v.z, 1.0f);
				world_coords[j] = vec3f(w.x, w.y, w.z);
			}

			vec3f N = (world_coords[2] - world_coords[0]).cross(world_coords[1] - world_coords[0]);
			N = N.normalize();

			float intensity = std::max(0.0f, light_dir.dot(N));

			Color c = white * intensity;
			triangle(&screen_coords[0], m_Color, m_Depth, RST_COLOR_ARGB(c.R, c.G, c.B, c.A));
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