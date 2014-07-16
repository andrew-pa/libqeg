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

		
		auto mos = mouse::get_state();
		{
			static vec2 dr = vec2(0.f);
			static float lnx = 0.f;
			static float lny = 0.f;
			float nx = mos.pos.x;
			float ny = mos.pos.y;
			float dnx = nx - lnx;
			float dny = ny - lny;
			dr = vec2(dnx, dny)*(pi*2);
			rot = dr/dt;
			lnx = nx;
			lny = ny;
		}
		
		if(gp != nullptr && 
			gp->connected())
		{
			auto gs = gp->get_state();
			move.x = gs.left_stick().y*ms;
			move.y = gs.dpad_stick().y*ms;
			move.z = (gs.left_stick().x + gs.dpad_stick().x)*ms;

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
		if (ks.key_down(input::key::page_up))
			ra += (5.f)*dt;
		if (ks.key_down(input::key::page_down))
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

	const vec3 ctg[6] =
	{
		vec3(-1.f, 0.f, 0.f),
		vec3(1.f, 0.f, 0.f),
		vec3(0.f, 1.f, 0.f),
		vec3(0.f, -1.f, 0.f),
		vec3(0.f, 0.f, 1.f),
		vec3(0.f, 0.f, -1.f),
	};

	const vec3 cup[6] =
	{
		vec3(0.f, 1.f, 0.f),
		vec3(0.f, 1.f, 0.f),
		vec3(0.f, 0.f, -1.f),
		vec3(0.f, 0.f, 1.f),
		vec3(0.f, 1.f, 0.f),
		vec3(0.f, 1.f, 0.f),
	};

	cubemap_camera_rig::cubemap_camera_rig(vec3 p, vec2 ss, float fov_, float nz_, float fz_)
		: _pos(p), _fov(fov_), _nz(nz_), _fz(fz_)
	{
		for (int i = 0; i < 6; ++i)
		{
			//vec3 t = _pos+ctg[i];
			camera c(_pos, ctg[i], _fov, ss, cup[i], _nz, _fz);
			//c.look_at(_pos, t, cup[i]); //redundant step? why?
			c.update_view();
			c.update_proj(ss);
			_cameras.push_back(c);
		}
	}

	void cubemap_camera_rig::update()
	{
		for (int i = 0; i < 6; ++i)
		{
			_cameras[i].position() = _pos;
		}
	}

	void cubemap_camera_rig::update_proj(vec2 s)
	{
		for (int i = 0; i < 6; ++i) _cameras[i].update_proj(s);
	}

	void cubemap_camera_rig::update_view()
	{
		for (int i = 0; i < 6; ++i) _cameras[i].update_view();
	}
}