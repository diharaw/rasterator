#include <math/vec3.hpp>
#include <math/vec2.hpp>
#include <math/mat4.hpp>

#include <vector>
#include <string>

using namespace math;

namespace rst
{
	struct Vertex
	{
		vec3f position;
		vec3f normal;
		vec2f texcoord;
	};

	struct Model
	{
		uint32_t			  num_vertices = 0;
		uint32_t			  num_indices  = 0;
		std::vector<Vertex>   vertices;
		std::vector<uint32_t> indices;
	};

	extern bool create_model(const std::string& obj, Model& model);
	extern void draw(Model& model, const mat4f& m, const mat4f& v, const mat4f& p);
}