#pragma once
#include <cmmn.h>
#include <shader.h>
#include <texture.h>
#include <constant_buffer.h>

using namespace qeg;

class sky_shader : public shader
{
	constant_buffer<mat4> wvp_mat;
	textureCube* sky_texture;
public:
	sky_shader(device* _dev)
		: shader(_dev, read_data_from_package(L"sky.vs.csh"), read_data_from_package(L"sky.ps.csh")), wvp_mat(_dev, *this, 0, mat4(), shader_stage::vertex_shader) {}
	void set_wvp(const mat4& m)
	{
		wvp_mat.data() = m;
	}

	void set_texture(textureCube* t)
	{
		sky_texture = t;
	}

	void bind(device* _dev) override
	{
		shader::bind(_dev);
		sky_texture->bind(_dev, 0, shader_stage::pixel_shader, *this);
		wvp_mat.bind(_dev);
	}

	void update(device* _dev) override
	{
		shader::update(_dev);
		wvp_mat.update(_dev);
	}

	void unbind(device* _dev) override
	{
		shader::unbind(_dev);
		sky_texture->unbind(_dev, 0, shader_stage::pixel_shader);
		wvp_mat.unbind(_dev);
	}
};