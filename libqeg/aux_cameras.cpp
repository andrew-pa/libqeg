#include "aux_cameras.h"
#include "basic_input.h"

namespace qeg
{
	using namespace input;

	void fps_camera::update(float dt)
	{
		auto ks = keyboard::get_state();

		vec3 move = vec3(0);
		vec2 rot = vec2(0);

		if(gp != nullptr && 
			gp->connected())
		{
			auto gs = gp->get_state();
			move.x = gs.left_stick().y*ms;
			move.y = gs.dpad_stick().y*ms;
			move.z = gs.left_stick().x*ms;

			rot = gs.right_stick()*vec2(1.f, -1.f)*rs;
		}

		if (ks.key_down(input::key::key_w))
			move.x = ms;
		if (ks.key_down(input::key::key_s))
			move.x = -ms;

		if (ks.key_down(input::key::key_a))
			move.z = -ms;
		if (ks.key_down(input::key::key_d))
			move.z = ms;

		if (ks.key_down(input::key::key_q))
			move.y = ms;
		if (ks.key_down(input::key::key_e))
			move.y = -ms;

		if (ks.key_down(input::key::up))
			rot.y = -rs;
		if (ks.key_down(input::key::down))
			rot.y = rs;

		if (ks.key_down(input::key::left))
			rot.x = -rs;
		if (ks.key_down(input::key::right))
			rot.x = rs;

		forward(move.x * dt);
		straft (move.z * dt);
		position().y += move.y * dt;

		pitch(rot.y * dt);
		transform(glm::rotate(mat4(1), rot.x*dt, vec3(0, 1, 0)));
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
		auto ks = keyboard::get_state();
		if (ks.key_down(input::key::left))
			th += (45.f)*dt;
		if (ks.key_down(input::key::right))
			th -= (45.f)*dt;
		if (ks.key_down(input::key::up))
			ph -= (45.f)*dt;
		if (ks.key_down(input::key::down))
			ph += 45.f*dt;
		if (ks.key_down(input::key::next))
			ra += (5.f)*dt;
		if (ks.key_down(input::key::prior))
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