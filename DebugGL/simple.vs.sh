#version 400 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;

layout(std140) uniform vs_reg_0
{
	mat4 wvp;
	mat4 inw;
	vec4 t;
};

out vec3 posW;
out vec3 normW;
out vec2 texc;

void main()
{
	vec4 v = vec4(pos + norm*vec3(sin(t.x*.2 + pos.z*3.0), 
	cos(t.x*.6 + pos.y * 3.0), sin(t.x*.3 + pos.x*3.0)) , 1);
	vec4 p = wvp*v;
	gl_Position = p;
	posW = pos;
	normW = (inw*vec4(norm, 0)).xyz;
	texc = tex;
}      