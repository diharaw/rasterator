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
#define RST_COLOR_ARGB(RED, GREEN, BLUE, ALPHA) (static_cast<uint32_t>(BLUE * 255.0f) << 24) | (static_cast<uint32_t>(GREEN * 255.0f) << 16) | (static_cast<uint32_t>(RED * 255.0f) << 8) | static_cast<uint32_t>(ALPHA * 255.0f)
#define RST_COLOR_RGBA(RED, GREEN, BLUE, ALPHA) (static_cast<uint32_t>(RED * 255.0f) << 24) | (static_cast<uint32_t>(GREEN * 255.0f) << 16) | static_cast<uint32_t>(BLUE * 255.0f) << 8 | static_cast<uint32_t>(ALPHA * 255.0f)

namespace rst
{
	class Color;

	class Texture
	{
	public:
		Color*	  m_pixels;
		float*	  m_depth;
		uint32_t  m_width;
		uint32_t  m_height;

	public:
		Texture(uint32_t width, uint32_t height, bool depth = false);
		Texture(const std::string& name);
		~Texture();
		void set_depth(float depth, uint32_t x, uint32_t y);
		void set_color(uint32_t color, uint32_t x, uint32_t y);
		uint32_t sample(float x, float y);
		void clear();
		void clear(float r, float g, float b, float a);
	};

	class Color
	{
	public:
		union
		{
			uint32_t pixel;
			struct
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
				uint8_t a;
			};
		};

		Color(uint8_t _r = 255, uint8_t _g = 255, uint8_t _b = 255, uint8_t _a = 255);
		Color(uint32_t _pixel);
		Color operator + (const Color &c) const;
		Color operator - (const Color &c) const;
		Color operator * (float f) const;
	};

	struct Vertex
	{
		vec3f position;
		vec3f normal;
		vec3f tangent;
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
		Texture* tex;
		std::vector<Vertex>   vertices;
		std::vector<uint32_t> indices;
		std::vector<SubModel> submodels;
	};

	extern bool create_model(const std::string& file, Model& model);
	extern void draw(Model& model, const mat4f& m, const mat4f& v, const mat4f& p, Texture* color, Texture* depth);
}