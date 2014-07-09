#pragma once
#include <cmmn.h>
#include <shader.h>
#include <texture.h>
#include <constant_buffer.h>

using namespace qeg;

class simple_shader : public shader
{
public:
	struct vs_matxs
	{
		mat4 world;
		mat4 view_proj;
		mat4 inv_world;
	};
	struct mat
	{
		vec3 diffuse;
		float extra;
		vec4 specular;
		mat(vec3 dif = vec3(0), float spec_exp = 0.f, vec3 spc = vec3(1.f))
			: diffuse(dif), specular(spc, spec_exp){}

	};

	struct point_light
	{
		vec4 pos;
		vec4 col;
		point_light(vec3 p = vec3(0), vec3 c = vec3(0))
			: pos(p.x, p.y, p.z, -1.f), col(c.x, c.y, c.z, -1.f) {}
	};

	struct lighting_cbd
	{
		point_light lights[3];
		uint light_count;
		uvec3 extra;
	};
protected:
	constant_buffer<vs_matxs> vsmt_cb;
	constant_buffer<mat> mat_cb;
	constant_buffer<lighting_cbd> li_cb;
	constant_buffer<vec4> campos_cb;
	texture2d* _tex, *_old_tex;
	bool _tex_changed;
public:
	simple_shader(device* _dev)
		: shader(_dev, read_data_from_package(L"simple.vs.csh"), read_data_from_package(L"simple.ps.csh")),
		vsmt_cb(_dev, *this, 0, vs_matxs(), shader_stage::vertex_shader),
		mat_cb(_dev, *this, 0, mat(), shader_stage::pixel_shader),
		li_cb(_dev, *this, 1, lighting_cbd(), shader_stage::pixel_shader),
		campos_cb(_dev, *this, 2, vec4(0.f), shader_stage::pixel_shader), _tex_changed(false), _tex(nullptr), _old_tex(nullptr)
	{
	}

	inline void world(const mat4& m)
	{
		vsmt_cb.data().world = m;
		vsmt_cb.data().inv_world = inverse(m);
	}

	inline void view_proj(const mat4& vp)
	{
		vsmt_cb.data().view_proj = vp;
	}

	inline void material(const mat& m)
	{
		mat_cb.data() = m;
	}

	inline void diffuse_tex(texture2d* t)
	{
		_old_tex = _tex;
		_tex = t;
		_tex_changed = true;
	}

	inline void light(const point_light& l, int idx)
	{
		if (idx > 3) throw exception();
		li_cb.data().lights[idx] = l;
	}

	inline void light_count(uint count)
	{
		li_cb.data().light_count = count;
	}

	inline void camera_position(vec3 p)
	{
		campos_cb.data() = vec4(p, 0.f);
	}

	void bind(device* _dev) override
	{
		shader::bind(_dev);
		vsmt_cb.bind(_dev);
		mat_cb.bind(_dev);
		li_cb.bind(_dev);
		campos_cb.bind(_dev);
	}

	void update(device* _dev) override
	{
		shader::update(_dev);
		vsmt_cb.update(_dev);
		mat_cb.update(_dev);
		li_cb.update(_dev);
		campos_cb.update(_dev);
		if (_tex_changed)
		{
			if (_old_tex != nullptr)
				_old_tex->unbind(_dev, 0, shader_stage::pixel_shader);
			if (_tex != nullptr)
				_tex->bind(_dev, 0, shader_stage::pixel_shader, *this);
		}
	}

	void unbind(device* _dev) override
	{
		shader::unbind(_dev);
		vsmt_cb.unbind(_dev);
		mat_cb.unbind(_dev);
		li_cb.unbind(_dev);
		campos_cb.unbind(_dev);
	}
};
