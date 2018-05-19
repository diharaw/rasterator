#include <rasterator.hpp>
#include <iostream>
#include <stdio.h>
#include <algorithm>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace rst
{
	Color::Color(float r, float g, float b, float a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}

	Color Color::operator + (const Color &c) const
	{
		return Color(R + c.R, G + c.G, B + c.B, A + c.A);
	}

	Color Color::operator - (const Color &c) const
	{
		return Color(R - c.R, G - c.G, B - c.B, A - c.A);
	}

	Color Color::operator * (float f) const
	{
		return Color(R * f, G * f, B * f, A * f);
	}

	Texture::Texture(uint32_t width, uint32_t height, bool depth) : m_width(width), m_height(height)
	{
		if (depth)
		{
			m_pixels = nullptr;
			m_depth = new float[width * height];
		}
		else
		{
			m_pixels = new uint32_t[width * height];;
			m_depth = nullptr;
		}
	}

	Texture::Texture(const std::string& name)
	{

	}

	Texture::~Texture()
	{
		RST_SAFE_DELETE_ARRAY(m_pixels);
		RST_SAFE_DELETE_ARRAY(m_depth);
	}

	void Texture::set_depth(float depth, uint32_t x, uint32_t y)
	{
		y = m_height - y - 1;

		if (m_depth)
		{
			if (x > m_width - 1 || x < 0)
				return;

			if (y > m_height - 1 || y < 0)
				return;

			m_depth[y * m_width + x] = depth;
		}
	}

	void Texture::set_color(uint32_t color, uint32_t x, uint32_t y)
	{
		y = m_height - y - 1;

		if (m_pixels)
		{
			if (x > m_width - 1 || x < 0)
				return;

			if (y > m_height - 1 || y < 0)
				return;

			m_pixels[y * m_width + x] = color;
		}
	}

	uint32_t Texture::sample(float x, float y)
	{
		uint32_t x_coord = x * (m_width - 1);
		uint32_t y_coord = y * (m_height - 1);

		return m_pixels[y_coord * m_width + x_coord];
	}

	void Texture::clear()
	{
		if (m_depth)
		{
			uint32_t size = m_width * m_height;
			float depth = INFINITY;

			for (uint32_t i = 0; i < size; i++)
				m_depth[i] = depth;
		}
	}

	void Texture::clear(float r, float g, float b, float a)
	{
		if (m_pixels)
		{
			uint32_t color = RST_COLOR_ARGB(r, g, b, a);
			uint32_t size = m_width * m_height;

			for (uint32_t i = 0; i < size; i++)
				m_pixels[i] = color;
		}
	}

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

	inline vec2f convert_to_screen_space(const float& x, const float& y, const uint32_t& width, const uint32_t& height)
	{
		return vec2f(int((((x + 1) * width) * 0.5f) + 0.5f), int((((y + 1) * height) * 0.5f) + 0.5f));
	}

	inline void triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const mat4f& mvp, Texture* color_tex, Texture* depth_tex)
	{
		uint32_t width = color_tex->m_width;
		uint32_t height = color_tex->m_height;

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

		// Iterate over pixels in triangle bounding box
		for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
		{
			for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
			{
				// Calculate barycentric coordinates
				float w0 = edge_function(v1screen, v2screen, p);
				float w1 = edge_function(v2screen, v0screen, p);
				float w2 = edge_function(v0screen, v1screen, p);

				// Is the current pixel within the triangle?
				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
					w0 /= area;
					w1 /= area;
					w2 /= area;

					// Calculate interpolated pixel depth
					float z = v0ndc.z * w0 + v1ndc.z * w1 + v2ndc.z *w2;

					// Perform depth test
					if (z < depth_tex->m_depth[int(p.x + p.y * depth_tex->m_width)])
					{
						// Update depth buffer value if depth test is passesd
						depth_tex->m_depth[int(p.x + p.y * depth_tex->m_width)] = z;

						// Write new pixel color
						color_tex->set_color(RST_COLOR_RGBA(1.0f, 1.0f, 1.0f, 1.0f), p.x, p.y);
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