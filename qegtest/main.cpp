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
#include <texture2d.h>
#include <states.h>
using namespace qeg;


mesh* create_box(device* _dev, const string& name, float d, bool interleaved = false)
{
	typedef vertex_position_normal_texture dvertex;
	vertex_position_normal_texture* v = new vertex_position_normal_texture[24];

	float w2 = 0.5f*d;
	float h2 = 0.5f*d;
	float d2 = 0.5f*d;

	// Fill in the front face vertex data. 
	v[0] = dvertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = dvertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = dvertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = dvertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = dvertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = dvertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = dvertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = dvertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = dvertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = dvertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = dvertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = dvertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = dvertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = dvertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = dvertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = dvertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = dvertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = dvertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = dvertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = dvertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = dvertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = dvertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = dvertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = dvertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	uint16* i = new uint16[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	if(interleaved)
	{
		return new interleaved_mesh<vertex_position_normal_texture, uint16>(_dev, 
			vector<vertex_position_normal_texture>(v, &v[24]), vector<uint16>(i, &i[36]), name);
	}

	auto indexCount = 36;
	auto vertexCount = 24;
	vector<vec3> p, n;
	vector<vec2> t;
	for (int i = 0; i < 24; ++i)
	{
		p.push_back(v[i].pos);
		n.push_back(v[i].norm);
		t.push_back(v[i].tex);
	}
	return new mesh_psnmtx(_dev, p, n, t, vector<uint16>(i, i + 36), name);
}

mesh* create_sphere(device* device, float radius, uint sliceCount, uint stackCount, const string& mesh_name, bool interleaved = false)
{
	typedef vertex_position_normal_texture dvertex;

	vector<vec3> p, n;
	vector<vec2> t;
	std::vector<uint16> indices;

	dvertex topv(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	dvertex botv(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	p.push_back(vec3(0, radius, 0)); n.push_back(vec3(0, 1, 0)); t.push_back(vec2(0, 0));

	float dphi = pi / stackCount;
	float dtheta = 2.0f*pi / sliceCount;

	for (UINT i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i*dphi;
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			float theta = j*dtheta;
			vec3 pos;
			pos.x = radius * sinf(phi) * cosf(theta);
			pos.y = radius * cosf(phi);
			pos.z = radius * sinf(phi) * sinf(theta);
			p.push_back(pos);
			n.push_back(normalize(pos));
			t.push_back(vec2(theta / pi*2, phi / pi));
			/*auto P = XMLoadFloat3(&v.pos);
			XMStoreFloat3(&v.norm, XMVector3Normalize(P));
			v.texc = float2(theta / XM_2PI, phi / XM_PI);*/
			//vertices->push_back(v);
		}
	}

	p.push_back(vec3(0, -radius, 0)); n.push_back(vec3(0, -1, 0)); t.push_back(vec2(0, 1));

	//vertices->push_back(botv);

	for (uint16 i = 1; i <= sliceCount; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	uint16 bi = 1;
	uint16 rvc = sliceCount + 1;
	for (uint16 i = 0; i < stackCount - 2; ++i)
	{
		for (uint j = 0; j < sliceCount; ++j)
		{
			indices.push_back(bi + i*rvc + j);
			indices.push_back(bi + i*rvc + j + 1);
			indices.push_back(bi + (i + 1)*rvc + j);
			indices.push_back(bi + (i + 1)*rvc + j);
			indices.push_back(bi + (i*rvc + j + 1));
			indices.push_back(bi + (i + 1)*rvc + j + 1);
		}
	}

	uint16 spi = (uint16)p.size() - 1;
	bi = spi - rvc;
	for (uint i = 0; i < sliceCount; ++i)
	{
		indices.push_back(spi);
		indices.push_back(bi + i);
		indices.push_back(bi + i + 1);
	}

	if(interleaved)
	{
			vector<vertex_position_normal_texture> v;
			for (int i = 0; i < p.size(); ++i)
			{
				v.push_back(vertex_position_normal_texture(p[i], n[i], t[i]));
			}
			return new interleaved_mesh<vertex_position_normal_texture, uint16>(device, v, indices, mesh_name);
	}

	return new mesh_psnmtx(device, p, n, t, indices, mesh_name);
}


class qegtest_app : public app
{
	mesh* m;
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
	orbit_camera c;

	texture2d* tx;
	sampler_state smpl;
public:
	qegtest_app()
		: app(
#ifdef DIRECTX
			L"libqeg test (DirectX)", 
#elif OPENGL
			L"libqeg test (OpenGL)",
#endif
			vec2(640, 480), false),
		s(_dev,
#ifdef OPENGL
		read_data_from_package(L"simple.vs.glsl"), read_data_from_package(L"simple.ps.glsl")
#elif DIRECTX
		read_data_from_package(L"simple.vs.cso"), read_data_from_package(L"simple.ps.cso")
#endif
		),
		wvp_cb(_dev, s, 0, wvpcbd(), shader_stage::vertex_shader),
		c(45, 45, 5.f, vec3(0), radians(45.f), _dev->size())//(vec3(3, 2, -10), vec3(0, 0, 1), 45.f, _dev->size()),
		, smpl(_dev)
	{
		c.target(vec3(0, .1f, 0));

		m = create_sphere(_dev, 1, 64, 64, "s0", true); //create_box(_dev, "box0", 1);

		tx = texture2d::load_dds(_dev, read_data_from_package(L"test.dds"));

	}

	void update(float t, float dt) override
	{
		c.update(dt);
		c.update_view();
		auto world = mat4(1);// rotate(mat4(1), t * 100, vec3(.2f, .7f, .6f));
		wvp_cb.data(wvpcbd(c.projection()*c.view()*world, glm::inverse(transpose(world)), vec4(t)));
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
		wvp_cb.update(_dev);
		m->draw(_dev);
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