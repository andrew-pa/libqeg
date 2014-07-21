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

#include "debug_render2d.h"

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

vec3 readvec3(ifstream& i)
{
	float x, y, z;
	i >> x >> y >> z;
	return vec3(x, y, z);
}
bool v3e(vec3 a, vec3 b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}
bool v2e(vec2 a, vec2 b)
{
	return a.x == b.x && a.y == b.y;
}

class model
{
	vector<mesh*> meshes;
	function<void(device*, uint)> mat_bind;
	function<void(device*, uint)> mat_unbind;

	void draw(device* _dev)
	{
		for (int i = 0; i < meshes.size(); ++i)
		{
			mat_bind(_dev, i);
			meshes[i]->draw(_dev);
			mat_unbind(_dev, i);
		}
	}
};

sys_mesh<vertex_position_normal_texture, uint16> load_obj(const string& OBJfilename)
{
	sys_mesh<vertex_position_normal_texture, uint16> m;
#pragma region read obj
	vector<vec3> poss;
	vector<vec3> norms;
	vector<vec2> texcords;

	ifstream inf(OBJfilename);
	char comm[256] = { 0 };

	while (inf)
	{
		inf >> comm;
		if (!inf) break;
		if (strcmp(comm, "#") == 0) continue;
		else if (strcmp(comm, "v") == 0)
			poss.push_back(readvec3(inf));
		else if (strcmp(comm, "vn") == 0)
			norms.push_back(readvec3(inf));
		else if (strcmp(comm, "vt") == 0)
		{
			float u, v;
			inf >> u >> v;
			texcords.push_back(vec2(u, v));
		}
		else if (strcmp(comm, "f") == 0)
		{
			for (uint ifa = 0; ifa < 3; ++ifa)
			{
				vertex_position_normal_texture v;
				uint ip, in, it;
				inf >> ip;
				v.pos = poss[ip - 1];
				if ('/' == inf.peek())
				{
					inf.ignore();
					if ('/' != inf.peek())
					{
						inf >> it;
						v.tex = texcords[it - 1];
					}
					if ('/' == inf.peek())
					{
						inf.ignore();
						inf >> in;
						v.norm = norms[in - 1];
					}
				}

				auto iv = find_if(m.vertices.begin(), m.vertices.end(),
					[&](const vertex_position_normal_texture& a)
				{
					bool pe = v3e(v.pos, a.pos);
					bool ne = v3e(v.norm, a.norm);
					bool te = v2e(v.tex, a.tex);
					return pe && ne && te; 
				});
				if (iv == m.vertices.end())
				{
					m.indices.push_back(m.vertices.size());
					m.vertices.push_back(v);
				}
				else
				{
					m.indices.push_back(std::distance(m.vertices.begin(), iv));
				}
			}
		}
	}
#pragma endregion
	return m;
}

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
	mesh* knot;

	fps_camera cam;
	camera other_cam;

	input::gamepad ctrl0;

	rasterizer_state wireframe_rs;
	bool render_wireframe;
	float wireframe_timer;

	blend_state bs_weird;

	depth_stencil_state dss;

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
		vec2(1280, 960), 8, pixel_format::BGRA8_UNORM, pixel_format::D32_FLOAT_S8X24_UINT),

		shd(_dev), skshd(_dev), r2ds(_dev),
		
		cam(vec3(0, 3, -5), vec3(0.1f), radians(45.f), _dev->size(), 10.f, 2.f, ctrl0),
		other_cam(vec3(0, 4, -10), vec3(0.1f)-vec3(0,2,-5), radians(45.f), vec2(1024)),
		rcb_rig(vec3(0, 4, 0), vec2(1024)),
		ctrl0(0), 
		
		wireframe_rs(_dev, fill_mode::wireframe, cull_mode::none), render_wireframe(false),
		sky_rs(_dev, fill_mode::solid, cull_mode::front),
		
		bs_weird(_dev, 
			{
				blend_state::render_target_blend_state_desc(true, blend_factor::src_alpha,
				blend_factor::inv_src_alpha, blend_op::add, blend_factor::one,
				blend_factor::zero, blend_op::add, write_mask::enable_all),
			}),  

		ball_pos(0, 1, -6),
		
		ss(_dev),
		tex(*texture2d::load(_dev, read_data_from_package(L"checker.tex"))),
		sky(*textureCube::load(_dev, read_data_from_package(L"testcm.tex"))),
		portal_tex(_dev, uvec2(1024), pixel_format::RGBA32_FLOAT, pixel_format::D24_UNORM_S8_UINT),
		depth_tex(_dev, uvec2(1024), pixel_format::D32_FLOAT_S8X24_UINT), rcb(_dev, 1024),
		sky_dss(_dev, true, true, comparison_func::less_equal),
		dss(_dev, true, true, comparison_func::less, true, 0xff, 0xff, 
				depth_stencil_state::depthstencil_op(stencil_op::decr, 
				stencil_op::keep, stencil_op::incr, comparison_func::equal))
	{
		//generate scene meshes
		ball = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, generate_sphere<vertex_position_normal_texture,uint16>(1.f, 64, 64), "ball");
		ground = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, generate_plane<vertex_position_normal_texture,uint16>(vec2(32), vec2(16), vec3(0, -1.f, 0)), "ground");
		torus = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, generate_torus<vertex_position_normal_texture, uint16>(vec2(1.f, .5f), 64), "torus");
		portal = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev,
			generate_plane<vertex_position_normal_texture, uint16>(vec2(4), vec2(16), vec3(.5f, 0, .5f)), "portal");
		
		//auto g = load_obj("C:\\Users\\andre_000\\Source\\libqeg\\DebugGL\\knot.obj");
		bo_file bf(bo_file::file_type::generic);
		/*bf.chunks().push_back(bo_file::chunk(0,
			new datablob<byte>((byte*)g.vertices.data(), g.vertices.size()*sizeof(vertex_position_normal_texture))));
		bf.chunks().push_back(bo_file::chunk(1,
			new datablob<byte>((byte*)g.indices.data(), g.indices.size()*sizeof(uint16))));
		auto x = bf.write();
		FILE* f = fopen("knot.mdl", "wb");
		fwrite(x.data, x.length, 1, f);
		fclose(f);*/
		bf = bo_file(read_data(L"knot.mdl"));
		auto om = read_mesh<vertex_position_normal_texture,uint16>(bf.chunks()[0], bf.chunks()[1]);
		knot = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, om, "knot");


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
		//if (with_rendered_textures) shd.diffuse_tex(&portal_tex); //if this is the render to render the texture, then use the rendered texture
		//shd.world(translate(mat4(1), vec3(4, 2, 4)));
		//shd.material(simple_shader::mat(vec3(.9f, .9f, .9f), 0.f));
		//shd.update(_dev);
		//portal->draw(_dev);
		//if (with_rendered_textures) shd.diffuse_tex(&tex); //reset the texture if needed

		dss.bind(_dev, -1);
		//draw the ball
		shd.world(translate(mat4(1), ball_pos));
		shd.material(simple_shader::mat(vec3(.8f, .45f, 0.f), 64.f));
		shd.update(_dev);
		ball->draw(_dev);

		//draw the knot
		shd.world(translate(mat4(1), vec3(4,1.5f,6)));
		shd.material(simple_shader::mat(vec3(.0f, .45f, .8f), 64.f));
		shd.update(_dev);
		knot->draw(_dev);
		dss.unbind(_dev);
		
		//bind the 'weird' blend state
		bs_weird.bind(_dev);

		//draw the torus
		shd.world(rotate(translate(mat4(1), vec3(-4, 1.4f, 6)), t, vec3(.2f, .6f, .4f)));
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
			r2ds.set_transform(mat4(.2f, 0, 0, 0,
				0, .2f, 0, 0,
				0, 0, 1, 0,
				-.25f, .75f, 0, 1));
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

	~qegtest_app()
	{
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