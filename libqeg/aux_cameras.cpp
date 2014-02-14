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
		_up = vec3(0, 1, 0);
		_right = cross(_up, _look);
		update_view();
		update_proj(ss);
	}

	void orbit_camera::update(float dt)
	{
		if (input::keyboard::get_state().key_down(input::key::left))
			th += (45.f)*dt;
		if (input::keyboard::get_state().key_down(input::key::right))
			th -= (45.f)*dt;
		if (input::keyboard::get_state().key_down(input::key::up))
			ph -= (45.f)*dt;
		if (input::keyboard::get_state().key_down(input::key::down))
			ph += 45.f*dt;
		if (input::keyboard::get_state().key_down(input::key::next))
			ra += (5.f)*dt;
		if (input::keyboard::get_state().key_down(input::key::prior))
			ra -= 5.f*dt;
	}

	void orbit_camera::update_view()
	{
		_pos.x = ra*sinf(radians(ph))*cosf(radians(th));
		_pos.y = ra*cosf(radians(ph));
		_pos.z = ra*sinf(radians(ph))*sinf(radians(th));
		look_at(_pos, vec3(0), vec3(0,1,0));
		camera::update_view();
	}
}