#pragma once
#include "cmmn.h"
#include "camera.h"


namespace qeg
{
	class fps_camera :
		public camera
	{
		float ms, rs;
	public:
		fps_camera(vec3 p, vec3 tar, float fov_, vec2 ss, float mov_s, float rot_s, vec3 u = vec3(0, 1, 0), float nz_ = 0.01f, float fz_ = 1000.f)
			: camera(p,tar-p,fov_,ss,u,nz_,fz_) { }
		
		proprw(float, move_speed, { return ms; });
		proprw(float, rotate_speed, { return rs; });

		void update(float dt);
	};

	class orbit_camera :
		public camera
	{
		float th;
		float ph;
		float ra;
	public:
		orbit_camera(float t, float p, float r, vec3 tar, float fov_, vec2 ss, float nz_ = 0.01f, float fz_ = 1000.f);

		proprw(float, theta, { return th; });
		proprw(float, phi, { return ph; });
		proprw(float, radius, { return ra; });

		void update(float dt);
		void update_view() override;
	};
}

