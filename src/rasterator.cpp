#include <rasterator.hpp>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <unordered_map>

#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
			m_pixels = new uint32_t[width * height];
			m_depth = nullptr;
		}
	}

	Texture::Texture(const std::string& name)
	{
		int x, y, comp;

		//stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(name.c_str(), &x, &y, &comp, 4);

		m_width = x;
		m_height = y;

		m_pixels = new uint32_t[x * y];
		m_depth = nullptr;

		memcpy(m_pixels, data, sizeof(stbi_uc) * x * y * 4);

		stbi_image_free(data);
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

	static const aiTextureType kTextureTypes[] = 
	{
		aiTextureType_DIFFUSE,
		aiTextureType_SPECULAR,
		aiTextureType_AMBIENT,
		aiTextureType_EMISSIVE,
		aiTextureType_HEIGHT,
		aiTextureType_NORMALS,
		aiTextureType_SHININESS,
		aiTextureType_OPACITY,
		aiTextureType_DISPLACEMENT,
		aiTextureType_LIGHTMAP,
		aiTextureType_REFLECTION
	};

	static const char* kTextureTypeStrings[] = 
	{
		"aiTextureType_DIFFUSE",
		"aiTextureType_SPECULAR",
		"aiTextureType_AMBIENT",
		"aiTextureType_EMISSIVE",
		"aiTextureType_HEIGHT",
		"aiTextureType_NORMALS",
		"aiTextureType_SHININESS",
		"aiTextureType_OPACITY",
		"aiTextureType_DISPLACEMENT",
		"aiTextureType_LIGHTMAP",
		"aiTextureType_REFLECTION"
	};

	std::string get_texture_path(aiMaterial* a_Material, aiTextureType a_TextureType)
	{
		aiString path;
		aiReturn result = a_Material->GetTexture(a_TextureType, 0, &path);

		if (result == aiReturn_FAILURE)
			return "";
		else
		{
			std::string cppStr = std::string(path.C_Str());

			if (cppStr == "")
				return "";

			return cppStr;
		}
	}

	bool does_material_exist(const std::vector<unsigned int>& _Materials, const uint32_t& _CurrentMaterial)
	{
		for (auto it : _Materials)
		{
			if (it == _CurrentMaterial)
				return true;
		}

		return false;
	}

	bool create_model(const std::string& file, Model& model)
	{
		const aiScene* Scene;
		Assimp::Importer Importer;
		Scene = Importer.ReadFile(file, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		uint32_t mesh_count = Scene->mNumMeshes;
		uint32_t index_count = 0;

		aiMaterial* TempMaterial;
		uint8_t materialIndex = 0;

		//std::vector<Assimp_Material> temp_materials;
		std::vector<unsigned int> processedMatId;
		std::unordered_map<unsigned int, uint8_t> MatIDMapping;
		uint32_t unamedMats = 1;

		for (int i = 0; i < mesh_count; i++)
		{
			SubModel submodel;

			bool hasLeastOneTexture = false;

			submodel.num_indices = Scene->mMeshes[i]->mNumFaces * 3;
			submodel.base_index = index_count;

			index_count += submodel.num_indices;

			//if (!does_material_exist(processedMatId, Scene->mMeshes[i]->mMaterialIndex))
			//{
			//	Assimp_Material temp;

			//	TempMaterial = Scene->mMaterials[Scene->mMeshes[i]->mMaterialIndex];
			//	temp.mesh_name = std::string(Scene->mMeshes[i]->mName.C_Str());

			//	std::string albedo = get_texture_path(TempMaterial, aiTextureType_DIFFUSE);

			//	temp.albedo[0] = '\0';
			//	if (albedo != "")
			//	{
			//		std::replace(albedo.begin(), albedo.end(), '\\', '/');

			//		if (albedo.length() > 4 && albedo[0] != ' ')
			//		{
			//			hasLeastOneTexture = true;
			//			strncpy(temp.albedo, albedo.c_str(), 50);
			//		}
			//	}

			//	if (hasLeastOneTexture)
			//	{
			//		if (temp.mesh_name.length() == 0 || temp.mesh_name == "" || temp.mesh_name == " ")
			//			temp.mesh_name = "untitled_" + std::to_string(unamedMats++);

			//		processedMatId.push_back(Scene->mMeshes[i]->mMaterialIndex);
			//		submodel.material_index = materialIndex;
			//		MatIDMapping[Scene->mMeshes[i]->mMaterialIndex] = materialIndex;

			//		temp_materials.push_back(temp);
			//		materialIndex++;
			//	}
			//	else
			//	{
			//		submodel.material_index = 0;
			//	}
			//}
			//else // if already exists, find the internal ID it maps to.
			//{
			//	submodel.material_index = MatIDMapping[Scene->mMeshes[i]->mMaterialIndex];
			//}

			model.submodels.push_back(submodel);
		}

		/*if (load_data->header.material_count > 0)
			load_data->materials = new Assimp_Material[load_data->header.material_count];
		else
			load_data->materials = nullptr;

		for (int i = 0; i < temp_materials.size(); i++)
		{
			load_data->materials[i] = temp_materials[i];
		}*/

		aiMesh* TempMesh;
		int idx = 0;
		int vertexIndex = 0;

		for (int i = 0; i < mesh_count; i++)
		{
			TempMesh = Scene->mMeshes[i];

			for (int k = 0; k < Scene->mMeshes[i]->mNumVertices; k++)
			{
				Vertex vert;

				vert.position = vec3f(TempMesh->mVertices[k].x, TempMesh->mVertices[k].y, TempMesh->mVertices[k].z);
				vec3f n = vec3f(TempMesh->mNormals[k].x, TempMesh->mNormals[k].y, TempMesh->mNormals[k].z);
				vec3f t = vec3f(TempMesh->mTangents[k].x, TempMesh->mTangents[k].y, TempMesh->mTangents[k].z);
				vec3f b = vec3f(TempMesh->mBitangents[k].x, TempMesh->mBitangents[k].y, TempMesh->mBitangents[k].z);

				// @NOTE: Assuming right handed coordinate space
				if (n.cross(t).dot(b) < 0.0f)
					t = t * -1.0f; // Flip tangent

				vert.normal = n;
				vert.tangent = t;

				if (TempMesh->HasTextureCoords(0))
					vert.texcoord = vec2f(TempMesh->mTextureCoords[0][k].x, TempMesh->mTextureCoords[0][k].y);

				vertexIndex++;

				model.vertices.push_back(vert);
			}

			for (int j = 0; j < TempMesh->mNumFaces; j++)
			{
				model.indices.push_back(TempMesh->mFaces[j].mIndices[0]);
				model.indices.push_back(TempMesh->mFaces[j].mIndices[1]);
				model.indices.push_back(TempMesh->mFaces[j].mIndices[2]);
			}
		}

		model.tex = new Texture("african_head_diffuse.tga");

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

	inline void triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const mat4f& mvp, Texture* color_tex, Texture* depth_tex, Texture* diffuse = nullptr)
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

						// Interpolate texture coordinates
						vec2f texcoord = v0.texcoord * w0 + v1.texcoord * w1 + v2.texcoord * w2;

						//std::cout << "[ " << texcoord.x << ", " << texcoord.y << " ]" << std::endl;

						// Fetch texture sample
						uint32_t color = diffuse->sample(texcoord.x, texcoord.y);

						// Write new pixel color
						color_tex->set_color(color, p.x, p.y);
						//color_tex->set_color(RST_COLOR_RGBA(1.0f, 1.0f, 1.0f, 1.0f), p.x, p.y);
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
				triangle(model.vertices[model.indices[i]], model.vertices[model.indices[i + 1]], model.vertices[model.indices[i + 2]], mvp, color, depth, model.tex);
			}
		}

	}
}