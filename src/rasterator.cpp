#include <rasterator.hpp>
#include <iostream>
#include <stdio.h>
#include <algorithm>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace rst
{
	
	bool create_model(const std::string& file, Model& model)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file.c_str());

		if (!err.empty())
			std::cerr << err << std::endl;

		if (!ret)
			return false;

		uint32_t num_vertices = attrib.vertices.size() / 3;

		for (size_t v = 0; v < num_vertices; v++)
		{
			Vertex vert;

			vert.position.x = attrib.vertices[3 * v + 0];
			vert.position.y = attrib.vertices[3 * v + 1];
			vert.position.z = attrib.vertices[3 * v + 2];
			
			if (attrib.normals.size() > 0)
			{
				vert.normal.x = attrib.normals[3 * v + 0];
				vert.normal.x = attrib.normals[3 * v + 1];
				vert.normal.x = attrib.normals[3 * v + 2];
			}

			if (attrib.texcoords.size() > 0)
			{
				vert.texcoord.x = attrib.texcoords[2 * v + 0];
				vert.texcoord.y = attrib.texcoords[2 * v + 1];
			}
	
			model.vertices.push_back(vert);
		}

		for (size_t s = 0; s < shapes.size(); s++) 
		{
			SubModel submodel;
			submodel.base_index = model.indices.size();
	
			size_t index_offset = 0;

			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) 
			{
				size_t fv = shapes[s].mesh.num_face_vertices[f];

				for (size_t v = 0; v < fv; v++)
				{
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					submodel.num_indices++;
					model.indices.push_back(idx.vertex_index);
				}

				index_offset += fv;
			}

			submodel.material_index = shapes[s].mesh.material_ids[0];
			model.submodels.push_back(submodel);
		}

		return true;
	}

	inline float edge_function(const vec2f &a, const vec2f &b, const vec2f &c)
	{
		return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
	}

	inline vec3f barycentric(vec2f v0, vec2f v1, vec2f v2, vec2f p)
	{
		float w0 = edge_function(v1, v2, p);
		float w1 = edge_function(v2, v0, p);
		float w2 = edge_function(v0, v1, p);

		return vec3f(w0, w1, w2);
	}

	inline vec2f convert_to_screen_space(const float& x, const float& y, const uint32_t& width, const uint32_t& height)
	{
		return vec2f(int((((x + 1) * width) * 0.5f) + 0.5f), int((((y + 1) * height) * 0.5f) + 0.5f));
	}

	inline void triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const mat4f& mvp, Texture* color_tex, Texture* depth_tex)
	{
		uint32_t width = color_tex->m_Width;
		uint32_t height = color_tex->m_Height;

		// Convert to screen space
		vec4f v0ndc = mvp * vec4f(v0.position.x, v0.position.y, v0.position.z, 1.0f);
		vec4f v1ndc = mvp * vec4f(v1.position.x, v1.position.y, v1.position.z, 1.0f);
		vec4f v2ndc = mvp * vec4f(v2.position.x, v2.position.y, v2.position.z, 1.0f);

		// Perspective division
		v0ndc = v0ndc / v0ndc.w;
		v1ndc = v1ndc / v1ndc.w;
		v2ndc = v2ndc / v2ndc.w;

		// Screen coords
		vec2f v0screen = convert_to_screen_space(v0ndc.x, v0ndc.y, width, height);
		vec2f v1screen = convert_to_screen_space(v1ndc.x, v1ndc.y, width, height);
		vec2f v2screen = convert_to_screen_space(v2ndc.x, v2ndc.y, width, height);

		// Find triangle bounding box
		vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

		vec2f clamp(width - 1, height - 1);

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

	void draw(Model& model, const mat4f& m, const mat4f& v, const mat4f& p, Texture* color, Texture* depth)
	{
		mat4f mvp = p * v * m;

		for (uint32_t s = 0; s < model.submodels.size(); s++)
		{
			SubModel& submodel = model.submodels[s];

			#pragma omp parallel for
			for (int32_t i = 0; i < submodel.num_indices; i += 3)
			{
				triangle(model.vertices[model.indices[i]], model.vertices[model.indices[i + 1]], model.vertices[model.indices[i + 2]], mvp, color, depth);
			}
		}

	}
}