#define LINK
#include <cmmn.h>
#include <device.h>
#include <app.h>
#include <shader.h>
#include <mesh.h>
#include <constant_buffer.h>
#include <camera.h>
#include <aux_cameras.h>
#include <basic_input.h>
#include <texture.h>
#include <states.h>
#include <bo_file.h>
#include <resource.h>
#include <render_target.h>
using namespace qeg;

#include <glm/gtc/noise.hpp>

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
			: pos(p.x,p.y,p.z,-1.f), col(c.x,c.y,c.z,-1.f) {}
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
		if(_tex_changed)
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

const static vec4 diffuse_ramp_data[8] =
{
	vec4(0.f, 0.f, 0.f, 1.f),
	vec4(1.f, 0.f, 0.f, 1.f),
	vec4(0.f, 1.f, 0.f, 1.f),
	vec4(0.f, 0.f, 1.f, 1.f),
	vec4(1.f, 1.f, 0.f, 1.f),
	vec4(0.f, 1.f, 1.f, 1.f),
	vec4(1.f, 0.f, 1.f, 1.f),
	vec4(1.f, 1.f, 1.f, 1.f),
};
#include "render_texture_ex.h"
//Tested Now
//meshes, constant_buffer, camera(s), gamepad, keyboard, rasterizer_state, app, device, render_texture2d, timer,
//Work but too lazy to add full test
//blend_state, mouse
//Not yet tested
//textures, sampler_state, blend_state, texture loading, render_texture for real

class qegtest_app : public app
{
	simple_shader shd;

	mesh* ball;
	mesh* ground;
	mesh* torus;
	mesh* portal;

	fps_camera cam;
	camera other_cam;

	input::gamepad ctrl0;

	rasterizer_state wireframe_rs;
	bool render_wireframe;
	float wireframe_timer;

	blend_state bs_weird;

	vec3 ball_pos;

	texture2d tex;
	sampler_state ss;

	sky_shader skshd;
	textureCube sky;
	mesh* sky_mesh;
	rasterizer_state sky_rs;
	depth_stencil_state sky_dss;

	render_texture2d portal_tex;
public:
	qegtest_app()
		: app(
#ifdef DIRECTX
		"libqeg test (DirectX)", 
#elif OPENGL
		"libqeg test (OpenGL)",
#endif
		vec2(1280, 960), 8, false, 1.f / 60.f),

		shd(_dev), skshd(_dev), 
		
		cam(vec3(0, 2, -5), vec3(0.1f), radians(45.f), _dev->size(), 10.f, 2.f, ctrl0),
		other_cam(vec3(0, 4, -10), vec3(0.1f)-vec3(0,2,-5), radians(45.f), vec2(1024)),
		
		ctrl0(0), 
		
		wireframe_rs(_dev, fill_mode::wireframe, cull_mode::none), render_wireframe(false),
		sky_rs(_dev, fill_mode::solid, cull_mode::front),
		
		bs_weird(_dev, 
			{
				blend_state::render_target_blend_state_desc(true, blend_factor::src_alpha,
				blend_factor::inv_src_alpha, blend_op::add, blend_factor::one,
				blend_factor::zero, blend_op::add, write_mask::enable_all),
			}), 

		ball_pos(0, 1, 0),
		
		ss(_dev),
		tex(*texture2d::load(_dev, read_data_from_package(L"checker.tex"))),
		sky(*textureCube::load(_dev, read_data_from_package(L"testcm.tex"))),
		portal_tex(_dev, uvec2(1024)),
		
		sky_dss(_dev, true, true, comparison_func::less_equal)
	{
		//generate scene meshes
		ball = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, generate_sphere<vertex_position_normal_texture,uint16>(1.f, 64, 64), "ball");
		ground = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, generate_plane<vertex_position_normal_texture,uint16>(vec2(32), vec2(16), vec3(0, -1.f, 0)), "ground");
		torus = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, generate_torus<vertex_position_normal_texture, uint16>(vec2(1.f, .5f), 64), "torus");
		portal = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, generate_plane<vertex_position_normal_texture, uint16>(vec2(4), vec2(16), vec3(.5f, 0, .5f)), "portal");
		
		//generate sky dome mesh
		sky_mesh = new interleaved_mesh<vertex_position, uint16>(_dev, generate_sphere<vertex_position, uint16>(2.f, 32, 32), "sky");
		
		//initialize shader lights
		shd.light(simple_shader::point_light(vec3(0, 10, 0), vec3(.5f)), 0);
		shd.light(simple_shader::point_light(vec3(-10, 10, -7), vec3(.5f, .4f, .4f)), 1);
		shd.light(simple_shader::point_light(vec3(14, 10, 5), vec3(.4f, .4f, .5f)), 2);
		shd.light_count(3);

		tex.tex_sampler() = &ss;
		sky.tex_sampler() = &ss;
		portal_tex.tex_sampler() = &ss;
	}

	void update(float t, float dt) override
	{
		ctrl0.update();
		auto gs = ctrl0.get_state();
		auto ks = input::keyboard::get_state();
		
		//check to see if user wants to quit
		if (gs.button_down(input::gamepad::button::back) ||
			ks.key_down(input::key::escape))
			PostQuitMessage(0);

		//check wireframe toggle
		wireframe_timer += dt;
		if((gs.button_pressed(input::gamepad::button::X)) ||
			(wireframe_timer > .2f && ks.key_pressed(input::key::key_u)))
		{
			wireframe_timer = 0.f;
			render_wireframe = !render_wireframe;
		}
		
		//check blend state write mask toggle
		//tests blend_state::update
		//if((gs.is_button_down(input::gamepad::button::Y) && !prev_gs.is_button_down(input::gamepad::button::Y))
		//	|| ks.key_pressed(input::key::key_y))
		//{
		//	bs_weird.render_targets[0].writemask = write_mask::enable_green;
		//	bs_weird.update(_dev);
		//}

		
		//update the light that moves
		shd.light(simple_shader::point_light(vec3(sin(t)*3.f, 10, cos(t)*3.f), vec3(.5f)), 0);

		//update cameras
		cam.update(dt);
		cam.update_view();

		other_cam.look_at(vec3(cos(t*.4f)*8.f, 2+sin(t*.3f), sin(t*.4f)*8.f), vec3(0.1f), vec3(0, 1, 0)); //move the portal camera around in a circle
		other_cam.update_view();

	}
	
	void resized() override
	{
		cam.update_proj(_dev->size());
		other_cam.update_proj(vec2(1024));
	}

	void render(float t, float dt, camera& c, bool with_rendered_textures = false)
	{
		//render sky dome 
		//if (!with_rendered_textures) {
			sky_rs.bind(_dev);
			sky_dss.bind(_dev, 0);
			skshd.set_texture(&sky);
			skshd.bind(_dev);
			skshd.set_wvp(c.projection()*c.view()*translate(mat4(1), c.position()));
			skshd.update(_dev);
			sky_mesh->draw(_dev);
			skshd.unbind(_dev);
			sky_dss.unbind(_dev);
			sky_rs.unbind(_dev);
		//}

		if (render_wireframe) wireframe_rs.bind(_dev); //if wireframe on, then bind that rasterizer state

		shd.bind(_dev); //bind shader & set camera values
		shd.view_proj(c.projection()*c.view());
		shd.camera_position(c.position());

		//bind default texture to draw the ground with
		shd.diffuse_tex(&tex);
		shd.world(mat4(1));
		shd.material(simple_shader::mat(vec3(.6f, .6f, .6f), 0.f));
		shd.update(_dev);
		ground->draw(_dev);

		//draw the portal
		if (with_rendered_textures) shd.diffuse_tex(&portal_tex); //if this is the render to render the texture, then use the rendered texture
		shd.world(translate(mat4(1), vec3(4, 2, 4)));
		shd.material(simple_shader::mat(vec3(.9f, .9f, .9f), 0.f));
		shd.update(_dev);
		portal->draw(_dev);
		if (with_rendered_textures) shd.diffuse_tex(&tex); //reset the texture if needed

		//draw the ball
		shd.world(translate(mat4(1), ball_pos));
		shd.material(simple_shader::mat(vec3(.8f, .45f, 0.f), 64.f));
		shd.update(_dev);
		ball->draw(_dev);
		
		//bind the 'weird' blend state
		bs_weird.bind(_dev);

		//draw the torus
		shd.world(rotate(translate(mat4(1), vec3(-3, 1.4f, 3)), t, vec3(.2f, .6f, .4f)));
		shd.material(simple_shader::mat(vec3(.1f, .8f, .1f), 256.f));
		shd.update(_dev);
		torus->draw(_dev);

		//unbind the 'weird' blend state
		bs_weird.unbind(_dev);

		shd.unbind(_dev);

		if (render_wireframe) wireframe_rs.unbind(_dev); //unbind the wireframe rasterizer state

		//render sky dome 
		//if (with_rendered_textures) {
		//	sky_rs.bind(_dev);
		//	sky_dss.bind(_dev, 0);
		//	skshd.set_texture(&sky);
		//	skshd.bind(_dev);
		//	skshd.set_wvp(c.projection()*c.view()*translate(mat4(1), c.position()));
		//	skshd.update(_dev);
		//	sky_mesh->draw(_dev);
		//	skshd.unbind(_dev);
		//	sky_dss.unbind(_dev);
		//	sky_rs.unbind(_dev);
		//}
	}

	void render(float t, float dt) override 
	{
		_dev->push_render_target(&portal_tex);
		render(t, dt, other_cam, false);
		_dev->pop_render_target();
		render(t, dt, cam, true);
	}
};

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR cmdLine, int cmdShow)
{
	qegtest_app a;
	a.run();
}















//
////random code
//
////ray gen -> intersect -> shade -> final
//
//struct ray { vec3 o, d; vec2 screen_pos; uint lvl; };
//
//vector<ray> gen_ray()
//{
//}
//
//struct hitrec { float t; vec3 n; vec2 texc; ray r; };
//
//vector<hitrec> intersect(const vector<ray>& r)
//{
//}
//
//vector<vec3> shade(const vector<hitrec>& hrs, vector<ray>& nxt)
//{
//}
//
//template <typename T>
//vector<T> op(const vector<T>& a, const vector<T>& b, function<T(const T& a, const T& b)> fn)
//{
//	vector<T> res;
//
//	for (int i = 0; i < a.size() && i < b.size(); ++i)
//	{
//		res.push_back(fn(a[i], b[i]));
//	}
//
//	return res;
//}
//
//vector<vec3> raytrace()
//{
//	vector<vec3> frmbuffer;
//	auto rs = gen_ray();
//	auto hr = intersect(rs);
//	vector<ray> nrs; vector<vec3> surf1;
//	auto surf0 = shade(hr, nrs);
//	uint idx = 0;
//	while(idx < 10)
//	{
//		auto r = op<vec3>(surf0, (surf1 = shade(intersect(nrs), rs)), [&](const vec3& a, const vec3& b) { return a * b; });
//		frmbuffer = op<vec3>(frmbuffer, r, [&](const vec3& a, const vec3& b) { return a + b; });
//		swap(rs, nrs);
//		rs.clear();
//		swap(surf0, surf1);
//		surf1.clear();
//		idx++;
//	}
//	return frmbuffer;
//}
//
////0 surf indrct0
////surf*indrct0 surf1 indrct1
////surf*indrct0 + surf1*indrct1