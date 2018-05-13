#include <rasterator.hpp>
#include <iostream>
#include <stdio.h>

namespace rst
{
	bool create_model(const std::string& obj, Model& model)
	{
		FILE* f = fopen(obj.c_str(), "rb");

		if (!f)
		{
			std::cout << "Failed to open model" << std::endl;
			return false;
		}

		std::vector<vec3f> positions;
		std::vector<vec3f> normals;
		std::vector<vec2f> texcoords;

		while (true)
		{
			char line[128];
			int result = fscanf(f, "%s", line);

			

			if (result == EOF)
			{
				break;
			}
			// Vertices
			else if (strcmp(line, "v") == 0)
			{
				vec3f p;
				fscanf(f, "%f %f %f\n", &p.x, &p.y, &p.z);
				positions.push_back(p);
			}
			// Normals
			else if (strcmp(line, "vn") == 0)
			{
				vec3f n;
				fscanf(f, "%f %f %f\n", &n.x, &n.y, &n.z);
				normals.push_back(n);
			}
			// TexCoords
			else if (strcmp(line, "vt") == 0)
			{
				vec2f t;
				float z;
				fscanf(f, "%f %f %f\n", &t.x, &t.y, &z);
				texcoords.push_back(t);
			}
			// Faces
			else if (strcmp(line, "f") == 0)
			{
				int position_idx[3];
				int texcoord_idx[3];
				int normal_idx[3];
				
				fscanf(f, "%i/%i/%i %i/%i/%i %i/%i/%i\n", &position_idx[0], &texcoord_idx[0], &normal_idx[0], 
														  &position_idx[1], &texcoord_idx[1], &normal_idx[1], 
														  &position_idx[2], &texcoord_idx[2], &normal_idx[2]);

				for (int i = 0; i < 3; i++)
				{
					Vertex v;

					v.position = positions[position_idx[i]];
					v.normal = normals[normal_idx[i]];
					v.texcoord = texcoords[texcoord_idx[i]];

					model.vertices.push_back(v);
				}
			}
		}

		fclose(f);

		return true;
	}

	void draw(Model& model, const mat4f& m, const mat4f& v, const mat4f& p)
	{
		uint32_t num_faces = model.num_indices / 3;
		mat4f mvp = p * v * m;

		for (uint32_t i = 0; i < num_faces; i += 3)
		{
			// Retrieve vertices of the current triangle
			Vertex v0 = model.vertices[model.indices[i]];
			Vertex v1 = model.vertices[model.indices[i + 1]];
			Vertex v2 = model.vertices[model.indices[i + 2]];

			// Project to screen
			vec4f p0 = mvp * vec4f(v0.position.x, v0.position.y, v0.position.z, 1.0f);
			vec4f p1 = mvp * vec4f(v1.position.x, v1.position.y, v1.position.z, 1.0f);
			vec4f p2 = mvp * vec4f(v2.position.x, v2.position.y, v2.position.z, 1.0f);

			// Perspective division
			p0 = p0 / p0.w;
			p1 = p1 / p1.w;
			p2 = p2 / p2.w;
		}
	}
}