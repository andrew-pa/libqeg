#define LINK
#include <cmmn.h>
#include <device.h>
#include <app.h>
#include <shader.h>
#include <mesh.h>
#include <constant_buffer.h>
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


#include <DirectXMath.h>
mat4x4 dx_perspectiveFov(float fov, float width, float height, float znear, float zfar)
{
	auto m = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), width / height, znear, zfar);
	mat4 rm(
		m.r[0].m128_f32[0], m.r[1].m128_f32[0], m.r[2].m128_f32[0], m.r[3].m128_f32[0],
		m.r[0].m128_f32[1], m.r[1].m128_f32[1], m.r[2].m128_f32[1], m.r[3].m128_f32[1],
		m.r[0].m128_f32[2], m.r[1].m128_f32[2], m.r[2].m128_f32[2], m.r[3].m128_f32[2],
		m.r[0].m128_f32[3], m.r[1].m128_f32[3], m.r[2].m128_f32[3], m.r[3].m128_f32[3]);
	return rm;
}





class qegtest_app : public app
{
	//ComPtr<ID2D1SolidColorBrush> brush;

	mesh* m;
	shader s;
	//constant_buffer<mat4> wvp_cb;
	vec3 cam_pos;

	GLuint u_wvpidx;
public:
	qegtest_app()
		: app(L"libqeg test", vec2(640, 480), true),
		s(_dev, read_data_from_package(L"simple.vs.sh"), read_data_from_package(L"simple.ps.sh")
#ifdef DIRECTX
		, shader::layout_posnomtex, 3
#endif
			)
		//wvp_cb(_dev, s, 0, mat4(1), shader_stage::vertex_shader)
	{
		check_gl
		//wvp_cb.data(
		//	perspectiveFov(45.f, 640.f, 480.f, .1f, 1000.f) * lookAt(vec3(0, 0, -15), vec3(0, 0, 0), vec3(0, 1, 0)));
		cam_pos = vec3(0, 5, -15);
		//_dev->d2context()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GreenYellow), &brush);		
		m = create_box(_dev, "box0", 1);
#ifdef OPENGL
		u_wvpidx = glGetUniformLocation(s.program_id(), "u_wvp");
	/*	glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);
		glEnable(GL_DEPTH_CLAMP);*/
#elif DIRECTX
		ID3D11RasterizerState* rs;
		CD3D11_RASTERIZER_DESC rsd(D3D11_FILL_SOLID, D3D11_CULL_BACK, TRUE, 0, 0, 0, TRUE, FALSE, FALSE, FALSE);
		
		_dev->ddevice()->CreateRasterizerState(&rsd, &rs);
		_dev->context()->RSSetState(rs);
#endif
	}

	void update(float t, float dt) override
	{
		if (GetAsyncKeyState('W') & 0x8000 == 0x8000)
		{
			cam_pos.z += 1;
		}
		if (GetAsyncKeyState('S') & 0x8000 == 0x8000)
		{
			cam_pos.z -= 1;
		}
		if (GetAsyncKeyState('A') & 0x8000 == 0x8000)
		{
			cam_pos.x += 1;
		}
		if (GetAsyncKeyState('D') & 0x8000 == 0x8000)
		{
			cam_pos.x -= 1;
		}
		auto wvp = perspectiveFov(45.f, 640.f, 480.f, .1f, 1000.f) * lookAt(cam_pos, vec3(0, 0, 0), vec3(0, 1, 0));
		glUniformMatrix4fv(u_wvpidx, 1, GL_FALSE, &wvp[0][0]);
		//wvp_cb.data(wvp);
	}
	
	void resized() override
	{
	}

	void render(float t, float dt) override
	{
		s.bind(_dev);
	//	wvp_cb.bind(_dev);
	//	wvp_cb.update(_dev);
		s.update(_dev);
		m->draw(_dev);
	//	wvp_cb.unbind(_dev);
		s.unbind(_dev);
		//_dev->d2context()->BeginDraw();
		//_dev->d2context()->DrawRectangle(D2D1::RectF(50, 50, 100, 100), brush.Get(), 5);
		//_dev->d2context()->EndDraw();
	}
};

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR cmdLine, int cmdShow)
{
	qegtest_app a;
	a.run();
}