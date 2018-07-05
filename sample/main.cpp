#include <application.hpp>
#include <math/vec3.hpp>
#include <rasterator.hpp>
#include <math/transform.hpp>
#include <math/utility.hpp>

#include <iostream>
#include <climits>
#include <algorithm>

class Demo : public Application
{
	mat4f m_view;
	mat4f m_projection;
	mat4f m_model;
	mat4f m_vp;
	mat4f m_mvp;
	vec3f m_position;
	vec3f m_direction;

	rst::DirectionalLight m_dir_light;
	rst::PointLight m_point_light;
	rst::Model m_obj_model;
	std::unique_ptr<rst::Texture> m_color_tex;
	std::unique_ptr<rst::Texture> m_depth_tex;

private:


protected:
	bool initialize() override
	{
		m_color_tex = std::make_unique<rst::Texture>(m_width, m_height);
		m_depth_tex = std::make_unique<rst::Texture>(m_width, m_height, true);

		m_position = vec3f(0.0f, 35.0f, 150.0f);
		m_direction = vec3f(0.0f, 0.0f, -1.0f);

		m_view = lookat(m_position, m_position + m_direction, vec3f(0.0f, 1.0f, 0.0f));
		m_projection = perspective(float(m_width) / float(m_height), radians(60.0f), 0.1f, 100.0f);
		m_vp = m_projection * m_view;

		if (!rst::create_model("teapot.obj", m_obj_model))
		{
			std::cout << "failed to load mesh" << std::endl;
			return false;
		}

		rst::initialize();

		m_dir_light.color = vec3f(1.0f, 1.0f, 1.0f);
		m_dir_light.direction = vec3f(1.0f, -1.0f, 0.0f).normalize();

		m_point_light.color = vec3f(1.0f, 1.0f, 1.0f);
		m_point_light.position = vec3f(0.0f, 0.0f, 150.0f);
		m_point_light.constant = 1.0f;
		m_point_light.linear = 0.0014;
		m_point_light.quadratic = 0.000007;

		return true;
	}

	void frame() override
	{
		m_depth_tex->clear();
		m_color_tex->clear(0.0f, 0.0f, 0.0f, 1.0f);

		m_model = rotation(radians(SDL_GetTicks() * 0.05f), vec3f(0.0f, 1.0f, 0.0f));

		// Set lights
		rst::set_directional_lights(1, &m_dir_light);

		// Set render targets
		rst::set_render_target(m_color_tex.get(), m_depth_tex.get());

		// Set buffers
		rst::set_vertex_buffer(&m_obj_model.vertex_buffer);
		rst::set_index_buffer(&m_obj_model.index_buffer);

		// Set matrices
		rst::set_projection_matrix(m_projection);
		rst::set_view_matrix(m_view);
		rst::set_model_matrix(m_model);

		// For each submodel in model...
		for (const auto& submodel : m_obj_model.submodels)
		{
			// Bind material, if available
			if (submodel.material)
			{
				rst::set_texture(rst::TEXTURE_DIFFUSE, submodel.material->diffuse);
				rst::set_texture(rst::TEXTURE_NORMAL, submodel.material->normal);
				rst::set_texture(rst::TEXTURE_SPECULAR, submodel.material->specular);
			}

			// Draw each submodel
			rst::draw_indexed_base_vertex(submodel.index_count, submodel.base_index, submodel.base_vertex);
		}

		update_backbuffer(m_color_tex->m_pixels);
	}

	void shutdown() override
	{

	}
};

RST_DECLARE_MAIN(Demo);
