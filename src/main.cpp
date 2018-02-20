#include "Application.h"

#include "model.h"
#include "geometry.h"

#include <climits>
#include <algorithm>

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
		TE_SAFE_DELETE_ARRAY(m_Pixels);
		TE_SAFE_DELETE_ARRAY(m_Depth);
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
			float depth = -std::numeric_limits<float>::max();

			for (uint32_t i = 0; i < size; i++)
				m_Depth[i] = depth;
		}
	}

	void Clear(float r, float g, float b, float a)
	{
		if (m_Pixels)
		{
			uint32_t color = COLOR_ARGB(r, g, b, a);
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

private:

	Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) 
	{
		Vec3f s[2];
		for (int i = 2; i--; ) {
			s[i][0] = C[i] - A[i];
			s[i][1] = B[i] - A[i];
			s[i][2] = A[i] - P[i];
		}
		Vec3f u = cross(s[0], s[1]);
		if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
			return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
		return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
	}

	void triangle(Vec3f *pts, Texture* colorTex, Texture* depthTex, uint32_t color)
	{
		Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
		Vec2f clamp(colorTex->m_Width - 1, colorTex->m_Height - 1);

		for (int i = 0; i<3; i++)
		{
			for (int j = 0; j<2; j++)
			{
				bboxmin[j] = std::max(0.0f, std::min(bboxmin[j], pts[i][j]));
				bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
			}
		}

		Vec3f P;

		for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
		{
			for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
			{
				Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);

				if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
					continue;

				P.z = 0;

				for (int i = 0; i < 3; i++)
					P.z += pts[i][2] * bc_screen[i];

				if (depthTex->m_Depth[int(P.x + P.y * colorTex->m_Width)] < P.z)
				{
					depthTex->m_Depth[int(P.x + P.y * colorTex->m_Width)] = P.z;
					colorTex->SetColor(color, P.x, P.y);
				}
				else
				{
					P.z++;
				}
			}
		}
	}

	Vec3f world2screen(Vec3f v, int width, int height) {
		return Vec3f(int((v.x + 1.)*width / 2. + .5), int((v.y + 1.)*height / 2. + .5), v.z);
	}

protected:
	bool Init() override
	{
		model = new Model("african_head.obj");

		m_Color = new Texture(m_Width, m_Height);
		m_Depth = new Texture(m_Width, m_Height, true);

		return true;
	}

	void Frame() override
	{
		m_Depth->Clear();
		m_Color->Clear(0.0f, 0.0f, 0.0f, 1.0f);

		Vec3f light_dir = Vec3f(0.0f, 0.0f, -1.0f);
		light_dir.normalize();

		for (int i = 0; i<model->nfaces(); i++) {
			std::vector<int> face = model->face(i);

			Vec3f screen_coords[3];
			Vec3f world_coords[3];

			for (int j = 0; j<3; j++) {
				Vec3f v = model->vert(face[j]);
				screen_coords[j] = world2screen(v, m_Color->m_Width, m_Color->m_Height);
				world_coords[j] = v;
			}

			Vec3f N = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
			N.normalize();

			float intensity = N * light_dir;

			if (intensity > 0)
			{
				Color c = white * intensity;
				triangle(&screen_coords[0], m_Color, m_Depth, COLOR_ARGB(c.R, c.G, c.B, c.A));
			}
		}

		UpdateBackBuffer(m_Color->m_Pixels);
	}

	void Shutdown() override
	{
		TE_SAFE_DELETE(model);
		TE_SAFE_DELETE(m_Depth);
		TE_SAFE_DELETE(m_Color);
	}
};

TE_DECLARE_MAIN(Demo);