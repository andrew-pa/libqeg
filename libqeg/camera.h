#pragma once
#include "cmmn.h"

namespace qeg
{
	class camera
	{
		mat4 _proj;
		mat4 _view;
		vec3 _pos;
		vec3 _look;
		vec3 _up;
		vec3 _right;
		float _fov;
		float _nz;
		float _fz;
	public:
		camera(vec3 p, vec3 l, float fov_, vec2 ss, vec3 u = vec3(0, 1, 0), float nz_ = 0.01f, float fz_ = 0.01f);
		camera(){}

		void update_proj(vec2 size);
		void update_view();

		inline void look_at(vec3 p, vec3 targ, vec3 u) { _pos = p; target(targ); _up = u; }

		propr(mat4, projection, { return _proj; });
		propr(mat4, view, { return _view; });

		proprw(vec3, position, { return _pos; });
		proprw(vec3, look, { return _look; });
		propr(vec3, target, { return _pos + _look; });
		inline void target(vec3 p) { _look = p - _pos; }
		proprw(vec3, up, { return _up; });
		proprw(vec3, right, { return _right; });
		proprw(float, fov, { return _fov; });
		proprw(float, near_z, { return _nz; });
		proprw(float, far_z, { return _fz; });

		inline void forward(float d) { _pos += d * _look;  }
		inline void straft(float d)  { _pos += d * _right; }
		inline void move_up(float d) { _pos += d * _up;    }
		
		void transform(mat4 t);
		
		inline void yaw(float a)   { transform(glm::rotate(mat4(1), a, _up));    }
		inline void pitch(float a) { transform(glm::rotate(mat4(1), a, _right)); }
		inline void roll(float a)  { transform(glm::rotate(mat4(1), a, _look));  }
		
		inline void rotate_ypr(vec3 a) 
		{
			mat4 r(1);
			glm::rotate(r, a.x, _up);
			glm::rotate(r, a.y, _right);
			glm::rotate(r, a.z, _look);
			transform(r);
		}

	};
}

