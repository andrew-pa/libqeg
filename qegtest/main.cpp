#define LINK
#include <cmmn.h>
#include <device.h>
#include <app.h>
#include <shader.h>
#include <mesh.h>
#include <constant_buffer.h>
#include <camera.h>
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



static const char* pVS = "                                                    \n\
						 #version 330                                                                  \n\
						                                                                               \n\
																									   layout (location = 0) in vec3 Position;                                       \n\
																									                                                                                 \n\
																																													 void main()                                                                   \n\
																																													 {                                                                             \n\
																																													     gl_Position = vec4(0.5 * Position.x, 0.5 * Position.y, Position.z, 1.0);  \n\
																																														 }";

static const char* pFS = "                                                    \n\
						 #version 330                                                                  \n\
						                                                                               \n\
																									   out vec4 FragColor;                                                           \n\
																									                                                                                 \n\
																																													 void main()                                                                   \n\
																																													 {                                                                             \n\
																																													     FragColor = vec4(1.0, 0.0, 0.0, 1.0);                                     \n\
																																														 }";

class qegtest_app : public app
{
	//ComPtr<ID2D1SolidColorBrush> brush;

	mesh* m;
	shader s;
	constant_buffer<mat4> wvp_cb;
	camera c;
	//GLuint shaderp;
	//GLuint VBO;

	//void add_shader(const char* st, GLenum ty)
	//{
	//	GLuint so = glCreateShader(ty);
	//	const GLchar* p[1];
	//	p[0] = st;
	//	GLint* len = new GLint(strlen(st));
	//	glShaderSource(so, 1, p, len);
	//	glCompileShader(so);
	//	GLint success;
	//	glGetShaderiv(so, GL_COMPILE_STATUS, &success);
	//	if (!success) {
	//		GLchar InfoLog[1024];
	//		glGetShaderInfoLog(so, 1024, NULL, InfoLog);
	//		OutputDebugStringA(InfoLog);
	//	}

	//	glAttachShader(shaderp, so);
	//}

public:
	qegtest_app()
		: app(L"libqeg test", vec2(640, 480), false),
		s(_dev, read_data_from_package(L"simple.vs.cso"), read_data_from_package(L"simple.ps.cso")
#ifdef DIRECTX
		, shader::layout_posnomtex, 3
#endif
			),
			wvp_cb(_dev, s, 0, mat4(1), shader_stage::vertex_shader),
			c(vec3(3, 2, -10), vec3(0, 0, 1), 45.f, _dev->size())
	{
		c.target(vec3(0, .1f, 0));
	
#ifdef OPENGL
#endif
		
		//shaderp = glCreateProgram();
		//add_shader(pVS, GL_VERTEX_SHADER);
		//add_shader(pFS, GL_FRAGMENT_SHADER);
		//glLinkProgram(shaderp);    
		//GLint Success;
		//glGetProgramiv(shaderp, GL_LINK_STATUS, &Success);
		//if (Success == 0) {
		//	GLchar ErrorLog[1024] = { 0 };
		//	glGetProgramInfoLog(shaderp, sizeof(ErrorLog), NULL, ErrorLog);
		//	OutputDebugStringA(ErrorLog);
		//}
		//glValidateProgram(shaderp);
		//glGetProgramiv(shaderp, GL_LINK_STATUS, &Success);
		//if (Success == 0) {
		//	GLchar ErrorLog[1024] = { 0 };
		//	glGetProgramInfoLog(shaderp, sizeof(ErrorLog), NULL, ErrorLog);
		//	OutputDebugStringA(ErrorLog);
		//}
		//glUseProgram(shaderp);
		m = create_box(_dev, "box0", 1);
		//vec3 Vertices[3];
		//Vertices[0] = vec3(-1.0f, -1.0f, 0.0f);
		//Vertices[1] = vec3(1.0f, -1.0f, 0.0f);
		//Vertices[2] = vec3(0.0f, 1.0f, 0.0f);

		//glGenBuffers(1, &VBO);
		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
#ifdef DIRECTX
		ID3D11RasterizerState* rs;
		CD3D11_RASTERIZER_DESC rsd(D3D11_FILL_SOLID, D3D11_CULL_BACK, TRUE, 0, 0, 0, TRUE, FALSE, FALSE, FALSE);
		
		_dev->ddevice()->CreateRasterizerState(&rsd, &rs);
		_dev->context()->RSSetState(rs);
#endif
	}

	void update(float t, float dt) override
	{
		if ((GetAsyncKeyState('W') & 0x8000) == 0x8000)
			c.forward(5.f*dt);
		if ((GetAsyncKeyState('S') & 0x8000) == 0x8000)
			c.forward(-5.f*dt);

		if ((GetAsyncKeyState('A') & 0x8000) == 0x8000)
			c.straft(-5.f*dt);
		if ((GetAsyncKeyState('D') & 0x8000) == 0x8000)
			c.straft(5.f*dt);

		if ((GetAsyncKeyState('Q') & 0x8000) == 0x8000)
			c.position().y += 5.f*dt;
		if ((GetAsyncKeyState('E') & 0x8000) == 0x8000)
			c.position().y -= 5.f*dt;

		if ((GetAsyncKeyState(VK_UP) & 0x8000) == 0x8000)
			c.pitch(-30.f*dt);
		if ((GetAsyncKeyState(VK_DOWN) & 0x8000) == 0x8000)
			c.pitch(30.f*dt);

		if ((GetAsyncKeyState(VK_LEFT) & 0x8000) == 0x8000)
			c.transform(glm::rotate(mat4(1), -30.f*dt, vec3(0, 1, 0)));
		if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) == 0x8000)
			c.transform(glm::rotate(mat4(1), 30.f*dt, vec3(0, 1, 0)));


		c.update_view();
		wvp_cb.data(c.projection()*c.view());//perspectiveFov(45.f, _dev->size().x, _dev->size().y, 0.01f, 1000.f) 
			//*lookAt(vec3(3, 2, -5), vec3(0, 0.2f, 0), vec3(0, 1, 0)));
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