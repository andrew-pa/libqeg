#include <cmmn.h>
#include <shader.h>
#include <constant_buffer.h>
#include <texture.h>

class render2d_shader : public shader
{
	constant_buffer<mat4> transform_mat;
	constant_buffer<uvec4> options_cb;

	//texture2d* tex, *oldtex;
	//bool _texchanged;
	texture_holder<2> tex;
	texture_holder<2> envtex;
	depth_stencil_state dss;
	rasterizer_state rss;
public:

	render2d_shader(device* _dev)
		: shader(_dev, read_data_from_package(L"render2d.vs.csh"), read_data_from_package(L"render2d.ps.csh")),
		transform_mat(_dev, *this, 0, mat4(), shader_stage::vertex_shader),
		options_cb(_dev, *this, 0, uvec4(0U), shader_stage::pixel_shader),
		//oldtex(nullptr), _texchanged(false),
		tex(0, shader_stage::pixel_shader), envtex(1, shader_stage::pixel_shader),
		dss(_dev, false), rss(_dev, fill_mode::solid, cull_mode::none)
	{}

	void set_transform(const mat4& m)
	{
		transform_mat.data() = m;
	}

	void set_texture(texture2d* t)
	{
		tex.set(t);
		//oldtex = tex;
		//tex = t;
		options_cb.data().y = 0;
		//_texchanged = true;
	}
	void set_texture(textureCube* t)
	{
		envtex.set(t);
		options_cb.data().y = 1;
	}

	void is_depth_texture(bool isadepthtexture)
	{
		options_cb.data().x = isadepthtexture ? 1U : 0U;
	}

	void bind(device* _dev) override
	{
		shader::bind(_dev);
		transform_mat.bind(_dev);
		options_cb.bind(_dev);
		dss.bind(_dev, 0);
		rss.bind(_dev);
	}

	void update(device* _dev) override
	{
		shader::update(_dev);
		transform_mat.update(_dev);
		options_cb.update(_dev);
		tex.bind(_dev, *this);
		envtex.bind(_dev, *this);
		/*if(_texchanged)
		{
		if (oldtex != nullptr)
		oldtex->unbind(_dev, 0, shader_stage::pixel_shader);
		if (tex != nullptr)
		tex->bind(_dev, 0, shader_stage::pixel_shader, *this);
		}*/
	}

	void unbind(device* _dev) override
	{
		shader::unbind(_dev);
		tex.unbind(_dev);
		envtex.unbind(_dev);
		//tex->unbind(_dev, 0, shader_stage::pixel_shader);
		transform_mat.unbind(_dev);
		options_cb.unbind(_dev);
		dss.unbind(_dev);
		rss.unbind(_dev);
	}
};

template <typename vertex_type, typename index_type>
sys_mesh<vertex_type, index_type> generate_screen_quad(vec2 offset, vec2 size)
{
	sys_mesh<vertex_type, index_type> m;

	const vec2 z[] =
	{
		vec2(1, 1),
		vec2(1, -1),
		vec2(-1, -1),
		vec2(-1, 1),
	};

	m.vertices.push_back(vertex_type(vec3(offset + size*z[0], 0), vec3(0, 1, 0),
		vec3(1, 0, 0), abs((z[0] - vec2(1.f))*vec2(.5f, .5f))));
	m.vertices.push_back(vertex_type(vec3(offset + size*z[1], 0), vec3(0, 1, 0),
		vec3(1, 0, 0), abs((z[1] - vec2(1.f))*vec2(.5f, .5f))));
	m.vertices.push_back(vertex_type(vec3(offset + size*z[2], 0), vec3(0, 1, 0),
		vec3(1, 0, 0), abs((z[2] - vec2(1.f))*vec2(.5f, .5f))));
	m.vertices.push_back(vertex_type(vec3(offset + size*z[3], 0), vec3(0, 1, 0),
		vec3(1, 0, 0), abs((z[3] - vec2(1.f))*vec2(.5f, .5f))));

	m.indices.push_back(0);
	m.indices.push_back(1);
	m.indices.push_back(2);

	m.indices.push_back(2);
	m.indices.push_back(3);
	m.indices.push_back(0);


	return m;
}