#include <math/vec3.hpp>
#include <math/vec2.hpp>
#include <math/mat4.hpp>

#include <memory>
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

	struct VertexBuffer
	{
		std::vector<Vertex> vertices;
	};

	struct IndexBuffer
	{
		std::vector<uint32_t> indices;
	};

	struct Material
	{
		Texture* diffuse;
		Texture* normal;
		Texture* specular;

		Material();
		~Material();
	};

	struct SubModel
	{
		uint32_t base_index = 0;
		uint32_t index_count = 0;
		uint32_t base_vertex = 0;
		Material* material;

		SubModel();
		~SubModel();
	};

	struct Model
	{
		std::vector<SubModel> submodels;
		std::vector<Material*> materials;
		VertexBuffer vertex_buffer;
		IndexBuffer index_buffer;

		Model();
		~Model();
	};

	enum TextureType
	{
		TEXTURE_DIFFUSE  = 0,
		TEXTURE_NORMAL   = 1,
		TEXTURE_SPECULAR = 2
	};

	struct DirectionalLight
	{
		vec3f direction;
		vec3f color;
	};

	struct PointLight
	{
		vec3f position;
		vec3f color;
		float constant;
		float linear;
		float quadratic;
	};

	extern bool create_model(const std::string& file, Model& model);
	extern void initialize();
	extern void set_vertex_buffer(VertexBuffer* vb);
	extern void set_index_buffer(IndexBuffer* ib);
	extern void set_directional_lights(uint32_t count, DirectionalLight* lights);
	extern void set_point_lights(uint32_t count, PointLight* lights);
	extern void set_render_target(Texture* color, Texture* depth);
	extern void set_model_matrix(const mat4f& model);
	extern void set_view_matrix(const mat4f& view);
	extern void set_projection_matrix(const mat4f& projection);
	extern void set_texture(const uint32_t& type, Texture* texture);
	extern void draw(uint32_t first_index, uint32_t count);
	extern void draw_indexed(uint32_t count);
	extern void draw_indexed_base_vertex(uint32_t index_count, uint32_t base_index, uint32_t base_vertex);
}