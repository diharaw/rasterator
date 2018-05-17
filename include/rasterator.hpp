#include <math/vec3.hpp>
#include <math/vec2.hpp>
#include <math/mat4.hpp>

#include <vector>
#include <string>

using namespace math;

#define RST_DECLARE_MAIN(APP_CLASS) \
int main(int argc, char* argv[])    \
{                                   \
    APP_CLASS app;                  \
    return app.run(argc, argv);     \
}

#define RST_SAFE_DELETE(OBJECT) if(OBJECT) { delete OBJECT; OBJECT = nullptr; }
#define RST_SAFE_DELETE_ARRAY(OBJECT) if(OBJECT) { delete[] OBJECT; OBJECT = nullptr; }
#define RST_COLOR_ARGB(RED, GREEN, BLUE, ALPHA) (static_cast<uint32_t>(ALPHA * 255.0f) << 24) | (static_cast<uint32_t>(RED * 255.0f) << 16) | (static_cast<uint32_t>(GREEN * 255.0f) << 8) | static_cast<uint32_t>(BLUE * 255.0f)
#define RST_COLOR_RGBA(RED, GREEN, BLUE, ALPHA) (static_cast<uint32_t>(RED * 255.0f) << 24) | (static_cast<uint32_t>(GREEN * 255.0f) << 16) | static_cast<uint32_t>(BLUE * 255.0f) << 8 | static_cast<uint32_t>(ALPHA * 255.0f)

namespace rst
{
	struct Vertex
	{
		vec3f position;
		vec3f normal;
		vec2f texcoord;
	};

	struct SubModel
	{
		uint32_t material_index = 0;
		uint32_t base_index = 0;
		uint32_t num_indices = 0;
	};

	struct Model
	{
		std::vector<Vertex>   vertices;
		std::vector<uint32_t> indices;
		std::vector<SubModel> submodels;
	};

	class Texture
	{
	public:
		uint32_t * m_Pixels;
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


	extern bool create_model(const std::string& file, Model& model);
	extern void draw(Model& model, const mat4f& m, const mat4f& v, const mat4f& p, Texture* color, Texture* depth);
}