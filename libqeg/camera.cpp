#include "camera.h"

namespace qeg
{
	camera::camera(vec3 p, vec3 l, float fov_, vec2 ss, vec3 u, float nz_, float fz_)
		: _pos(p), _look(l), _up(u), _fov(fov_), _nz(nz_), _fz(fz_), _view(1), _proj(1)
	{
		_right = cross(_up, _look); 
		update_view();
		update_proj(ss);
	}

	void camera::update_proj(vec2 size)
	{
		_proj = perspectiveFov(_fov, size.x, size.y, _nz, _fz);
	}

	void camera::update_view()
	{
		_look = normalize(_look);
		_up = normalize(cross(_look, _right));
		_right = cross(_up, _look);

		float px = -dot(_pos, _right);
		float py = -dot(_pos, _up);
		float pz = -dot(_pos, _look);

		_view = mat4(1);
		_view[0][0] = _right.x;
		_view[1][0] = _right.y;
		_view[2][0] = _right.z;
		_view[0][1] = _up.x;
		_view[1][1] = _up.y;
		_view[2][1] = _up.z;
		_view[0][2] = -_look.x;
		_view[1][2] = -_look.y;
		_view[2][2] = -_look.z;
		_view[3][0] = px;
		_view[3][1] = py;
		_view[3][2] = pz;
	}

	void camera::transform(mat4 t)
	{
		_look = vec3(vec4(_look, 0)*t);
		_up = vec3(vec4(_up, 0)*t);
		_right = vec3(vec4(_right, 0)*t);
		_pos = vec3(vec4(_pos, 1)*t);
	}
}