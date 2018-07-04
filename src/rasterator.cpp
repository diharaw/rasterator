#include <rasterator.hpp>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <SDL.h>

namespace rst
{
	// Global state.
	VertexBuffer* g_current_vb = nullptr;
	IndexBuffer*  g_current_ib = nullptr;
	Texture*	  g_current_color_target = nullptr;
	Texture*	  g_current_depth_target = nullptr;
	Texture*	  g_current_textures[3];
	mat4f		  g_current_model_mat;
	mat4f		  g_current_view_mat;
	mat4f		  g_current_projection_mat;

	// -----------------------------------------------------------------------------------------------------------------------------------

	Color::Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	Color::Color(uint32_t _pixel)
	{
		pixel = _pixel;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	Color Color::operator + (const Color &c) const
	{
		return Color(r + c.r, g + c.g, b + c.b, a + c.a);
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	Color Color::operator - (const Color &c) const
	{
		return Color(r - c.r, g - c.g, b - c.b, a - c.a);
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	Color Color::operator * (float f) const
	{
		return Color(r * f, g * f, b * f, a * f);
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	Texture::Texture(uint32_t width, uint32_t height, bool depth) : m_width(width), m_height(height)
	{
		if (depth)
		{
			m_pixels = nullptr;
			m_depth = new float[width * height];
		}
		else
		{
			m_pixels = new Color[width * height];
			m_depth = nullptr;
		}
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	Texture::Texture(const std::string& name)
	{
		int x, y, comp;

        std::string base_path = SDL_GetBasePath();
        std::string path = base_path + name;
		Color* data = (Color*)stbi_load(path.c_str(), &x, &y, &comp, 4);

		m_width = x;
		m_height = y;

		m_pixels = new Color[x * y];
		m_depth = nullptr;

		int size = x * y;

		for (int i = 0; i < size; i++)
		{
			Color& c = data[i];
			m_pixels[i] = Color(c.b, c.g, c.r, 255);
		}

		stbi_image_free(data);
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	Texture::~Texture()
	{
		RST_SAFE_DELETE_ARRAY(m_pixels);
		RST_SAFE_DELETE_ARRAY(m_depth);
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

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

	// -----------------------------------------------------------------------------------------------------------------------------------

	void Texture::set_color(uint32_t color, uint32_t x, uint32_t y)
	{
		y = m_height - y - 1;

		if (m_pixels)
		{
			if (x > m_width - 1)
				return;

			if (y > m_height - 1)
				return;

			m_pixels[y * m_width + x] = color;
		}
	}
    
	// -----------------------------------------------------------------------------------------------------------------------------------

    inline Color bilinear_interpolation(const float& tx, const float& ty, const Color& c00, const Color& c01, const Color& c10, const Color& c11)
    {
        Color a = c00 * (1 - tx) + c10 * tx;
        Color b = c01 * (1 - tx) + c11 * tx;
        return a * (1 - ty) + b * ty;
    }
#define BILINEAR
	uint32_t Texture::sample(float x, float y)
	{
#if defined(BILINEAR)
        // Bilinear Filtering
        float x_coord = x * (float(m_width) - 1.0f);
        float y_coord = y * (float(m_height) - 1.0f);
        
        // Get floor value of coordinate
        uint32_t x_floor = uint32_t(x_coord);
        uint32_t y_floor = uint32_t(y_coord);

        // Get ceil value of coordinate
        uint32_t x_ceil = ceil(x_coord);
        uint32_t y_ceil = ceil(y_coord);

        // Calculate tx, ty
        float tx = x_coord - x_floor;
        float ty = y_coord - y_floor;
        
        Color& c00 = m_pixels[y_floor * m_width + x_floor];
        Color& c01 = m_pixels[y_ceil * m_width + x_floor];
        Color& c10 = m_pixels[y_floor * m_width + x_ceil];
        Color& c11 = m_pixels[y_ceil * m_width + x_ceil];
        
        return bilinear_interpolation(tx, ty, c00, c01, c10, c11).pixel;
#else
        uint32_t x_coord = x * (m_width - 1);
        uint32_t y_coord = y * (m_height - 1);

        return m_pixels[y_coord * m_width + x_coord].pixel;
#endif
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

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

	// -----------------------------------------------------------------------------------------------------------------------------------

	void Texture::clear(float r, float g, float b, float a)
	{
		if (m_pixels)
		{
			Color color = Color(b * 255.0f, g * 255.0f, r * 255.0f, a * 255.0f);
			uint32_t size = m_width * m_height;

			for (uint32_t i = 0; i < size; i++)
				m_pixels[i] = color.pixel;
		}
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	Material::Material()
	{
		diffuse = nullptr;
		normal = nullptr;
		specular = nullptr;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	Material::~Material()
	{
		RST_SAFE_DELETE(diffuse);
		RST_SAFE_DELETE(normal);
		RST_SAFE_DELETE(specular);
	}					

	// -----------------------------------------------------------------------------------------------------------------------------------

	SubModel::SubModel()
	{
		material = nullptr;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	SubModel::~SubModel()
	{
		
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	Model::Model()
	{

	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	Model::~Model()
	{
		for (auto mat : materials)
			RST_SAFE_DELETE(mat);
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

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

	// -----------------------------------------------------------------------------------------------------------------------------------
	// Assimp loader helper method definitions
	// -----------------------------------------------------------------------------------------------------------------------------------

	std::string assimp_get_texture_path(aiMaterial* material, aiTextureType texture_type)
	{
		aiString path;
		aiReturn result = material->GetTexture(texture_type, 0, &path);

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

	// -----------------------------------------------------------------------------------------------------------------------------------

	bool assimp_does_material_exist(std::vector<unsigned int> &materials, unsigned int &current_material)
	{
		for (auto it : materials)
		{
			if (it == current_material)
				return true;
		}

		return false;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	bool create_model(const std::string& file, Model& model)
	{
		const aiScene* Scene;
		Assimp::Importer Importer;
        
        std::string base_path = SDL_GetBasePath();
        std::string path = base_path + file;
        
		Scene = Importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		uint32_t mesh_count = Scene->mNumMeshes;
		uint32_t index_count = 0;
		uint32_t vertex_count = 0;

		aiMaterial* TempMaterial;
		uint8_t materialIndex = 0;

		aiMaterial* temp_material;
		std::vector<unsigned int> processed_mat_id;
		std::unordered_map<unsigned int, Material*> mat_id_mapping;

		// Resize submodel vector
		model.submodels.resize(mesh_count);

		for (int i = 0; i < mesh_count; i++)
		{
			SubModel& submodel = model.submodels[i];

			bool has_least_one_texture = false;

			submodel.index_count = Scene->mMeshes[i]->mNumFaces * 3;
			submodel.base_index = index_count;
			submodel.base_vertex = vertex_count;

			vertex_count += Scene->mMeshes[i]->mNumVertices;
			index_count += submodel.index_count;

			std::string diffuse_path;
			std::string normal_path;
			std::string specular_path;

			if (mat_id_mapping.find(Scene->mMeshes[i]->mMaterialIndex) == mat_id_mapping.end())
			{
				std::string current_mat_name;

				temp_material = Scene->mMaterials[Scene->mMeshes[i]->mMaterialIndex];
				current_mat_name = path + std::to_string(i);

				// Find Diffuse
				diffuse_path = assimp_get_texture_path(temp_material, aiTextureType_DIFFUSE);

				if (diffuse_path != "")
				{
					std::replace(diffuse_path.begin(), diffuse_path.end(), '\\', '/');

					if (diffuse_path.length() > 4 && diffuse_path[0] != ' ')
					{
						std::cout << "Found Diffuse Map : " + diffuse_path << std::endl;
						has_least_one_texture = true;
					}
				}

				// Find Normal
				normal_path = assimp_get_texture_path(temp_material, aiTextureType_HEIGHT);

				if (normal_path != "")
				{
					std::replace(normal_path.begin(), normal_path.end(), '\\', '/');

					if (normal_path.length() > 4 && normal_path[0] != ' ')
					{
						std::cout << "Found Normal Map : " + normal_path << std::endl;
						has_least_one_texture = true;
					}
				}

				// Find Specular
				specular_path = assimp_get_texture_path(temp_material, aiTextureType_SPECULAR);

				if (specular_path != "")
				{
					std::replace(specular_path.begin(), specular_path.end(), '\\', '/');

					if (specular_path.length() > 4 && specular_path[0] != ' ')
					{
						std::cout << "Found Specular Map : " + specular_path << std::endl;
						has_least_one_texture = true;
					}
				}

				if (has_least_one_texture)
				{
					submodel.material = new Material();//std::make_unique<Material>();

					if (diffuse_path != "")
						submodel.material->diffuse = new Texture(diffuse_path);

					if (normal_path != "")
						submodel.material->normal = new Texture(normal_path);

					if (specular_path != "")
						submodel.material->specular = new Texture(specular_path);

					mat_id_mapping[Scene->mMeshes[i]->mMaterialIndex] = submodel.material;//submodel.material.get();
					model.materials.push_back(submodel.material);
				}

			}
			else // if already exists, find the pointer.
				submodel.material = mat_id_mapping[Scene->mMeshes[i]->mMaterialIndex];//std::unique_ptr<Material>(mat_id_mapping[Scene->mMeshes[i]->mMaterialIndex]);
		}

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

				model.vertex_buffer.vertices.push_back(vert);
			}

			for (int j = 0; j < TempMesh->mNumFaces; j++)
			{
				model.index_buffer.indices.push_back(TempMesh->mFaces[j].mIndices[0]);
				model.index_buffer.indices.push_back(TempMesh->mFaces[j].mIndices[1]);
				model.index_buffer.indices.push_back(TempMesh->mFaces[j].mIndices[2]);
			}
		}

		return true;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	inline float edge_function(const vec2f &a, const vec2f &b, const vec2f &c)
	{
		return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	inline vec2f convert_to_screen_space(const float& x, const float& y, const uint32_t& width, const uint32_t& height)
	{
		return vec2f(int((((x + 1) * width) * 0.5f) + 0.5f), int((((y + 1) * height) * 0.5f) + 0.5f));
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

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

						// Interpolate texture coordinates
						vec2f texcoord = v0.texcoord * w0 + v1.texcoord * w1 + v2.texcoord * w2;

						//std::cout << "[ " << texcoord.x << ", " << texcoord.y << " ]" << std::endl;

						// Fetch texture sample
						Texture* diffuse = g_current_textures[TEXTURE_DIFFUSE];
						uint32_t color = diffuse ? diffuse->sample(texcoord.x, texcoord.y) : RST_COLOR_RGBA(1.0f, 1.0f, 1.0f, 1.0f);

						// Write new pixel color
						color_tex->set_color(color, p.x, p.y);
					}
				}
			}
		}
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	void initialize()
	{
		for (int i = 0; i < 3; i++)
			g_current_textures[i] = nullptr;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	void set_vertex_buffer(VertexBuffer* vb)
	{
		g_current_vb = vb;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	void set_index_buffer(IndexBuffer* ib)
	{
		g_current_ib = ib;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	void set_render_target(Texture* color, Texture* depth)
	{
		g_current_color_target = color;
		g_current_depth_target = depth;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	void set_model_matrix(const mat4f& model)
	{
		g_current_model_mat = model;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	void set_view_matrix(const mat4f& view)
	{
		g_current_view_mat = view;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	void set_projection_matrix(const mat4f& projection)
	{
		g_current_projection_mat = projection;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	void set_texture(const uint32_t& type, Texture* texture)
	{
		if (type > TEXTURE_SPECULAR)
		{
			std::cout << "ERROR: Invalid texture type!" << std::endl;
			return;
		}

		g_current_textures[type] = texture;
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	void draw(uint32_t first_index, uint32_t count)
	{
		if (!g_current_vb)
		{
			std::cout << "DRAW ERROR: No vertex buffer bound!" << std::endl;
			return;
		}

		// Retrieve vertices vector from vertex buffer.
		std::vector<Vertex>& vertices = g_current_vb->vertices;

		// Compute MVP matrix.
		mat4f mvp = g_current_projection_mat * g_current_view_mat * g_current_model_mat;
		
		// Iterate over vertices.
		#pragma omp parallel for
		for (int i = 0; i < count; i += 3)
		{
			// Rasterize triangle.
			triangle(vertices[first_index + i], vertices[first_index + i + 1], vertices[first_index + i + 2], mvp, g_current_color_target, g_current_depth_target);
		}
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	void draw_indexed(uint32_t count)
	{
		if (!g_current_vb)
		{
			std::cout << "DRAW INDEXED ERROR: No vertex buffer bound!" << std::endl;
			return;
		}

		if (!g_current_ib)
		{
			std::cout << "DRAW INDEXED ERROR: No index buffer bound!" << std::endl;
			return;
		}

		// Retrieve vertices and indices vectors from vertex and index buffers.
		std::vector<uint32_t>& indices = g_current_ib->indices;
		std::vector<Vertex>& vertices = g_current_vb->vertices;

		// Compute MVP matrix.
		mat4f mvp = g_current_projection_mat * g_current_view_mat * g_current_model_mat;

		// Iterate over vertices,
		#pragma omp parallel for
		for (int i = 0; i < count; i += 3)
		{
			// Rasterize triangle.
			triangle(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]], mvp, g_current_color_target, g_current_depth_target);
		}
	}

	// -----------------------------------------------------------------------------------------------------------------------------------

	void draw_indexed_base_vertex(uint32_t index_count, uint32_t base_index, uint32_t base_vertex)
	{
		if (!g_current_vb)
		{
			std::cout << "DRAW INDEXED ERROR: No vertex buffer bound!" << std::endl;
			return;
		}

		if (!g_current_ib)
		{
			std::cout << "DRAW INDEXED ERROR: No index buffer bound!" << std::endl;
			return;
		}

		// Retrieve vertices and indices vectors from vertex and index buffers.
		std::vector<uint32_t>& indices = g_current_ib->indices;
		std::vector<Vertex>& vertices = g_current_vb->vertices;

		// Compute MVP matrix.
		mat4f mvp = g_current_projection_mat * g_current_view_mat * g_current_model_mat;

		// Iterate over vertices.
		#pragma omp parallel for
		for (int i = 0; i < index_count; i += 3)
		{
			// Rasterize triangle.
			triangle(vertices[base_vertex + indices[base_index + i]], vertices[base_vertex + indices[base_index + i + 1]], vertices[base_vertex + indices[base_index + i + 2]], mvp, g_current_color_target, g_current_depth_target);
		}
	}

	// -----------------------------------------------------------------------------------------------------------------------------------
} // namespace rst
