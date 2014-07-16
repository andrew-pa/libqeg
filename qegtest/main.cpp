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

#include "simple_shader.h"
#include "sky_shader.h"

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
		vec3(1, 0, 0), abs((z[0] - vec2(1.f))*vec2(.5f, .5f)) ));
	m.vertices.push_back(vertex_type(vec3(offset + size*z[1], 0), vec3(0, 1, 0),
		vec3(1, 0, 0), abs((z[1] - vec2(1.f))*vec2(.5f, .5f)) ));
	m.vertices.push_back(vertex_type(vec3(offset + size*z[2], 0), vec3(0, 1, 0),
		vec3(1, 0, 0), abs((z[2] - vec2(1.f))*vec2(.5f, .5f)) ));
	m.vertices.push_back(vertex_type(vec3(offset + size*z[3], 0), vec3(0, 1, 0),
		vec3(1, 0, 0), abs((z[3] - vec2(1.f))*vec2(.5f, .5f)) ));
	
	m.indices.push_back(0);
	m.indices.push_back(1);
	m.indices.push_back(2);

	m.indices.push_back(2);
	m.indices.push_back(3);
	m.indices.push_back(0);

	
	return m;
}

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

#include "render_target_ex.h"

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
	depth_render_texture2d depth_tex;

	mesh* quad_mesh;
	render2d_shader r2ds;

	render_textureCube rcb;
	cubemap_camera_rig rcb_rig;
public:
	qegtest_app()
		: app(
#ifdef DIRECTX
		"libqeg test (DirectX)", 
#elif OPENGL
		"libqeg test (OpenGL)",
#endif
		vec2(1280, 960), 8, false, 1.f / 60.f),

		shd(_dev), skshd(_dev), r2ds(_dev),
		
		cam(vec3(0, 3, -5), vec3(0.1f), radians(45.f), _dev->size(), 10.f, 2.f, ctrl0),
		other_cam(vec3(0, 4, -10), vec3(0.1f)-vec3(0,2,-5), radians(45.f), vec2(1024)),
		rcb_rig(vec3(0, 4, -10), vec2(1024)),
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
		depth_tex(_dev, uvec2(1024)), rcb(_dev, 1024),
		sky_dss(_dev, true, true, comparison_func::less_equal)
	{
		//generate scene meshes
		ball = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, generate_sphere<vertex_position_normal_texture,uint16>(1.f, 64, 64), "ball");
		ground = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, generate_plane<vertex_position_normal_texture,uint16>(vec2(32), vec2(16), vec3(0, -1.f, 0)), "ground");
		torus = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, generate_torus<vertex_position_normal_texture, uint16>(vec2(1.f, .5f), 64), "torus");
		portal = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev,
			generate_plane<vertex_position_normal_texture, uint16>(vec2(4), vec2(16), vec3(.5f, 0, .5f)), "portal");
		
		//generate sky dome mesh
		sky_mesh = new interleaved_mesh<vertex_position, uint16>(_dev, generate_sphere<vertex_position, uint16>(2.f, 32, 32), "sky");
		
		//generate quad mesh for drawing debug textures
		quad_mesh = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev,
			generate_screen_quad<vertex_position_normal_texture, uint16>(vec2(1.0f), vec2(1.f)), "quad_2d");;

		//initialize shader lights
		shd.light(simple_shader::point_light(vec3(0, 10, 0), vec3(.5f)), 0);
		shd.light(simple_shader::point_light(vec3(-10, 10, -7), vec3(.5f, .4f, .4f)), 1);
		shd.light(simple_shader::point_light(vec3(14, 10, 5), vec3(.4f, .4f, .5f)), 2);
		shd.light_count(3);

		tex.tex_sampler() = &ss;
		sky.tex_sampler() = &ss;
		portal_tex.tex_sampler() = &ss;
		depth_tex.tex_sampler() = &ss;
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

		other_cam.look_at(vec3(cos(t*.4f)*(10.f+sin(t*.2f)), 2+sin(t*.3f), sin(t*.4f)*8.f), vec3(0.1f), vec3(0, 1, 0)); //move the portal camera around in a circle
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
	/*	if (with_rendered_textures) {
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
		}*/

		if(with_rendered_textures) //make sure not to render 2d stuff on to render target
		{
			r2ds.bind(_dev);
			r2ds.set_transform(mat4(.2f, 0, 0, 0, 
									0, .2f, 0, 0,
									0, 0, 1, 0,
									.75f, .75f, 0, 1));
			r2ds.set_texture(&portal_tex);
			r2ds.is_depth_texture(false);
			r2ds.update(_dev);
			quad_mesh->draw(_dev);
			r2ds.unbind(_dev);

			r2ds.bind(_dev);
			r2ds.set_transform(mat4(.2f, 0, 0, 0,
				0, .2f, 0, 0,
				0, 0, 1, 0,
				.25f, .75f, 0, 1));
			r2ds.set_texture(&depth_tex);
			r2ds.is_depth_texture(true);
			r2ds.update(_dev);
			quad_mesh->draw(_dev);
			r2ds.unbind(_dev);
			r2ds.bind(_dev);
			r2ds.set_transform(mat4(.3f, 0, 0, 0,
				0, .3f, 0, 0,
				0, 0, 1, 0,
				-.45f, .65f, 0, 1));
			r2ds.set_texture(&rcb);
			r2ds.is_depth_texture(false);
			r2ds.update(_dev);
			quad_mesh->draw(_dev);
			r2ds.unbind(_dev);
		}
	}

	void render(float t, float dt) override 
	{
		//render to render target for 'portal' + upper right texture view
		_dev->push_render_target(&portal_tex);
		render(t, dt, other_cam, false);
		_dev->pop_render_target();

		//render to depth target for upper middle right texture view
		//	this currently generates a warning about having no render target set for the pixel shader, but that is because
		//	in theory have no pixel shader bound for a depth render, but right now that doesn't happen, this being the result
		//	: EXECUTION WARNING #3146081: DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET
		_dev->push_render_target(&depth_tex);
		render(t, dt, cam, false);
		_dev->pop_render_target();

		//render to each face of the texture cube
		for (int i = 0; i < 6; ++i)
		{
			auto rt = rcb.target_for_face(i);
			_dev->push_render_target(rt);
			render(t, dt, rcb_rig.camera_for_face(i), false);
			_dev->pop_render_target();
		}

		//render main view
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