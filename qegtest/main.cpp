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
using namespace qeg;


mesh* create_box(device* _dev, const string& name, float d)
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

mesh* create_sphere(device* device, float radius, qeg::uint sliceCount, qeg::uint stackCount, const string& mesh_name)
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
		for (qeg::uint j = 0; j < sliceCount; ++j)
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
	for (qeg::uint i = 0; i < sliceCount; ++i)
	{
		indices.push_back(spi);
		indices.push_back(bi + i);
		indices.push_back(bi + i + 1);
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
	float theta;
	float phi;

public:
	qegtest_app()
		: app(L"libqeg test", vec2(640, 480), false),
		s(_dev, 
#ifdef OPENGL
		read_data_from_package(L"simple.vs.sh"), read_data_from_package(L"simple.ps.sh")
#elif DIRECTX
		read_data_from_package(L"simple.vs.cso"), read_data_from_package(L"simple.ps.cso"), shader::layout_posnomtex, 3
#endif
			),
			wvp_cb(_dev, s, 0, wvpcbd(), shader_stage::vertex_shader),
			c(0, 0.1f, 5.f, vec3(0), 45.f, _dev->size()),//(vec3(3, 2, -10), vec3(0, 0, 1), 45.f, _dev->size()),
			theta(0), phi(0.1f)
	{
		c.target(vec3(0, .1f, 0));

		m = create_sphere(_dev, 1.f, 64, 64, "sphere0");//create_box(_dev, "box0", 1);
#ifdef DIRECTX
		ID3D11RasterizerState* rs;
		CD3D11_RASTERIZER_DESC rsd(D3D11_FILL_SOLID, D3D11_CULL_BACK, TRUE, 0, 0, 0, TRUE, FALSE, FALSE, FALSE);
		
		_dev->ddevice()->CreateRasterizerState(&rsd, &rs);
		_dev->context()->RSSetState(rs);
#endif
	}

	void update(float t, float dt) override
	{
		//if (input::keyboard::get_state().key_down(input::key::key_w))
		//	c.forward(5.f*dt);
		//if (input::keyboard::get_state().key_down(input::key::key_s))
		//	c.forward(-5.f*dt);

		//if (input::keyboard::get_state().key_down(input::key::key_a))
		//	c.straft(-5.f*dt);
		//if (input::keyboard::get_state().key_down(input::key::key_d))
		//	c.straft(5.f*dt);

		//if (input::keyboard::get_state().key_down(input::key::key_q))
		//	c.position().y += 5.f*dt;
		//if (input::keyboard::get_state().key_down(input::key::key_e))
		//	c.position().y -= 5.f*dt;

		//if (input::keyboard::get_state().key_down(input::key::up)) //(GetAsyncKeyState(VK_UP) & 0x8000) == 0x8000)
		//	c.pitch(-30.f*dt);
		//if (input::keyboard::get_state().key_down(input::key::down))
		//	c.pitch(30.f*dt);
		//
		//if (input::keyboard::get_state().key_down(input::key::left))
		//	c.transform(glm::rotate(mat4(1), -30.f*dt, vec3(0, 1, 0)));
		//if (input::keyboard::get_state().key_down(input::key::right))
		//	c.transform(glm::rotate(mat4(1), 30.f*dt, vec3(0, 1, 0)));

		c.update(dt);


		/*c.position().x = 5.f * sinf(radians(phi)) * cosf(radians(theta));
		c.position().z = 5.f * sinf(radians(phi)) * sinf(radians(theta));
		c.position().y = 5.f * cosf(radians(phi));
		c.look_at(c.position(), vec3(0), vec3(0, 1, 0));*/


		if(input::mouse::get_state().right)
		{
			c.fov() += 45.f*dt;
			c.update_proj(_dev->size());
		}
		if (input::mouse::get_state().left)
		{
			c.fov() -= 45.f*dt;
			c.update_proj(_dev->size());
		}
  
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
		wvp_cb.bind(_dev);
		wvp_cb.update(_dev);
		m->draw(_dev);
		wvp_cb.unbind(_dev);
		s.unbind(_dev);
	}
};

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR cmdLine, int cmdShow)
{
	qegtest_app a;
	a.run();
}