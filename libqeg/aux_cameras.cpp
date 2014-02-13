#include "aux_cameras.h"
#include "basic_input.h"

namespace qeg
{
	using namespace input;

	void fps_camera::update(float dt)
	{
		if (keyboard::get_state().key_down(input::key::key_w))
			forward(ms*dt);
		if (keyboard::get_state().key_down(input::key::key_s))
			forward(-ms*dt);

		if (keyboard::get_state().key_down(input::key::key_a))
			straft(-ms*dt);
		if (keyboard::get_state().key_down(input::key::key_d))
			straft(ms*dt);

		if (keyboard::get_state().key_down(input::key::key_q))
			position().y += ms*dt;
		if (keyboard::get_state().key_down(input::key::key_e))
			position().y -= ms*dt;

		if (keyboard::get_state().key_down(input::key::up))
			pitch(-rs*dt);
		if (keyboard::get_state().key_down(input::key::down))
			pitch(rs*dt);

		if (keyboard::get_state().key_down(input::key::left))
			transform(glm::rotate(mat4(1), -rs*dt, vec3(0, 1, 0)));
		if (keyboard::get_state().key_down(input::key::right))
			transform(glm::rotate(mat4(1), rs*dt, vec3(0, 1, 0)));
	}

	orbit_camera::orbit_camera(float t, float p, float r, vec3 tar, float fov_, vec2 ss, float nz_, float fz_)
		: camera(fov_, nz_, fz_), th(t), ph(p), ra(r)
	{
		target(tar);
		update_view();
		update_proj(ss);
	}

	void orbit_camera::update_view()
	{
		
		camera::update_view();
	}
}