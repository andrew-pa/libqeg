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
using namespace qeg;

#include <glm/gtc/noise.hpp>


inline texture1d* load_texture1d(device* dev, datablob<byte>* file_data) { return static_cast<texture1d*>(qeg::detail::_load_texture(dev, file_data)); }
inline texture2d* load_texture2d(device* dev, datablob<byte>* file_data) { return static_cast<texture2d*>(qeg::detail::_load_texture(dev, file_data)); }
inline texture3d* load_texture3d(device* dev, datablob<byte>* file_data) { return static_cast<texture3d*>(qeg::detail::_load_texture(dev, file_data)); }
inline textureCube* load_textureCube(device* dev, datablob<byte>* file_data) { return static_cast<textureCube*>(qeg::detail::_load_texture(dev, file_data)); }

class qegtest_app : public app
{
	mesh* m;
	mesh* ball;
	mesh* ground;

	shader s;
	struct wvpcbd
	{
		mat4 wvp;
		mat4 inw;
		vec4 t;
		wvpcbd() {}
		wvpcbd(mat4 a, mat4 b, vec4 _t)
			: wvp(a), inw(b), t(_t){}
	};
	constant_buffer<wvpcbd> wvp_cb;
	fps_camera c;

	texture2d* tx;
	texture1d* tx2;
	sampler_state smpl;

	input::gamepad ctrl0;
public:
	qegtest_app()
		: app(
#ifdef DIRECTX
			L"libqeg test (DirectX)", 
#elif OPENGL
			L"libqeg test (OpenGL)",
#endif
			vec2(640, 480), false, 1.f/60.f),
		s(_dev, read_data_from_package(L"simple.vs.csh"), read_data_from_package(L"simple.ps.csh")),
		wvp_cb(_dev, s, 0, wvpcbd(), shader_stage::vertex_shader),
		c(vec3(0.5f, 8.f, -28), vec3(0,0,0.1), radians(40.f), _dev->size(), 10.f, 1.5f, ctrl0) //(45, 45, 5.f, vec3(0), radians(45.f), _dev->size())//(vec3(3, 2, -10), vec3(0, 0, 1), 45.f, _dev->size()),
		, smpl(_dev), ctrl0(0)
	{

		m = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev,
			generate_torus<vertex_position_normal_texture, uint16>(vec2(2, .7f), 64), "torus0");
			//generate_plane<vertex_position_normal_texture, uint16>(vec2(16), vec2(64), vec3(0, -1, 0)), "plane0");
			//generate_sphere<vertex_position_normal_texture,uint16>(2.f, 64, 64), "sphere0"); 
			
		ball = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev,
			generate_sphere<vertex_position_normal_texture, uint16>(2.f, 64, 64), "sphere0");

		ground = new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev,
			generate_plane<vertex_position_normal_texture,uint16>(vec2(32), vec2(64), vec3(0.f,-1.f,0)), "ground");

		//tx = texture2d::load_dds(_dev, read_data_from_package(L"test.dds"));

		//const int size = 512;

		//bo_file f(bo_file::file_type::texture);
		//::detail::texture_header h;
		//h.dim = texture_dimension::texture_2d;
		//h.size = uvec3(size, size, 0);
		//h.array_count = 0;
		//h.mip_count = 10;
		//h.format = ::detail::pi_pixel_format::RGBA32_FLOAT;
		////vec4* img = new vec4[size * size];
		////for (int y = 0; y < size; ++y)
		////	for (int x = 0; x < size; x++)
		////	{
		////		vec2 p = vec2((float)x / (float)size, (float)y / (float)size);
		////		float n = abs(perlin(p*32.f));
		////		img[x + y * size] = vec4(vec3(n), 1.f);
		////	}
		//uint fsize = 0;
		//vector<size_t> offsets;
		//for (int i = 0; i < h.mip_count; ++i)
		//{
		//	uvec2 msize = (uvec2)floor(vec2(size) / pow(2.f, (float)i));
		//	offsets.push_back(fsize);
		//	fsize += msize.x * msize.y;		
		//}
		// 
		//vec4* img = new vec4[fsize];
		//vector<vec3> pal(20);
		//generate(pal.begin(), pal.end(), [&] { return vec3(linearRand(vec3(.2f), vec3(.8f))); });
		//for (int i = 0; i < h.mip_count; ++i)
		//{
		//	vec4* im = img + offsets[i];
		//	uvec2 msize = (uvec2)floor(vec2(size) / pow(2.f, (float)i));
		//	for (int y = 0; y < msize.y; ++y)
		//		for (int x = 0; x < msize.x; ++x)
		//		{
		//			vec2 p = vec2((float)x / (float)msize.x, (float)y / (float)msize.y);
		//			float n = abs(perlin(p*32.f));
		//			im[x + (y * msize.y)] = vec4(n, n*.5f, 0,1.f);
		//		}
		//}
		//
		//bo_file::chunk hc(0, new datablob<byte>((byte*)&h, sizeof(::detail::texture_header)));
		//bo_file::chunk dc(1, new datablob<byte>((byte*)img, fsize*sizeof(vec4)));
		//f.chunks().push_back(hc);
		//f.chunks().push_back(dc);
		//auto txf = f.write();
		//
		//ofstream out("perlin.tex", ios_base::binary);
		//out.write((const char*)txf->data, txf->length);
		//out.flush();
		//out.close();

		tx = texture2d::load(_dev, 
			read_data(L"C:\\Users\\andre_000\\Source\\libqeg\\qegtest\\perlin.tex")); //load_texture2d(_dev,
			//read_data(L"C:\\Users\\andre_000\\Source\\libqeg\\qegtest\\perlin.tex"));

	}

	void update(float t, float dt) override
	{
		if(input::gamepad::is_connected(0))
		{
			ctrl0.update();
		}

		c.update(dt);
		c.update_view();
	}
	
	void resized() override
	{
		c.update_proj(_dev->size());
	}

	void render(float t, float dt) override
	{
		s.bind(_dev);
		tx->bind(_dev, 0, shader_stage::pixel_shader, s);
		smpl.bind(_dev, 0, shader_stage::pixel_shader);
		wvp_cb.bind(_dev);

		auto world = translate(mat4(1), vec3(0, -1.f, 0));
		wvp_cb.data(wvpcbd(c.projection()*c.view()*world, glm::inverse(transpose(world)), vec4(t, 0, 0, 0)));
		wvp_cb.update(_dev);
		ground->draw(_dev);

		world = mat4(1);
		wvp_cb.data(wvpcbd(c.projection()*c.view()*world, glm::inverse(transpose(world)), vec4(t, 0, 0, 0)));
		wvp_cb.update(_dev);
		ball->draw(_dev);

		for (float i = 0; i < 6; ++i)
		{
		world = translate(mat4(1),
			vec3(10*cosf(i+t*.4f), sinf(i+t*.2f)*.2f, 10*sinf(i+t*.4f)));
		wvp_cb.data(wvpcbd(c.projection()*c.view()*world, 
			glm::inverse(transpose(world)), vec4(t, i, 0, 0)));
		wvp_cb.update(_dev);
		m->draw(_dev);
		}
		smpl.unbind(_dev, 0, shader_stage::pixel_shader);
		tx->unbind(_dev, 0, shader_stage::pixel_shader);
		wvp_cb.unbind(_dev);
		s.unbind(_dev);
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