#include <application.hpp>
#include <model.hpp>
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

	rst::Model m_obj_model;
	rst::Texture* m_color_tex;
	rst::Texture* m_depth_tex;

private:


protected:
	bool initialize() override
	{
		m_color_tex = new rst::Texture(m_width, m_height);
		m_depth_tex = new rst::Texture(m_width, m_height, true);

		m_position = vec3f(0.0f, 0.0f, 1.0f);
		m_direction = vec3f(0.0f, 0.0f, -1.0f);

		m_view = lookat(m_position, m_position + m_direction, vec3f(0.0f, 1.0f, 0.0f));
		m_projection = perspective(float(m_width) / float(m_height), radians(60.0f), 0.1f, 100.0f);
		m_vp = m_projection * m_view;

		if (!rst::create_model("dragon.obj", m_obj_model))
		{
			std::cout << "failed to load mesh" << std::endl;
			return false;
		}

		return true;
	}

	void frame() override
	{
		m_depth_tex->Clear();
		m_color_tex->Clear(0.0f, 0.0f, 0.0f, 1.0f);

		vec3f light_dir = vec3f(0.0f, 0.0f, -1.0f);
		light_dir.normalize();

		m_model = rotation(radians(SDL_GetTicks() * 0.05f), vec3f(0.0f, 1.0f, 0.0f));

		rst::draw(m_obj_model, m_model, m_view, m_projection, m_color_tex, m_depth_tex);

		update_backbuffer(m_color_tex->m_Pixels);
	}

	void shutdown() override
	{
		RST_SAFE_DELETE(m_depth_tex);
		RST_SAFE_DELETE(m_color_tex);
	}
};

RST_DECLARE_MAIN(Demo);