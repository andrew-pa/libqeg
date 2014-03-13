#version 400 core
layout(location=0) out vec4 color;
in vec3 posW;
in vec3 normW;
in vec2 texc;

uniform sampler2D ps_tex_0;

void main()
{
	color = vec4((texture(ps_tex_0, texc).xyz + abs(normW)*max(0, dot(normW, vec3(0, 1, 0)))) + abs(normW)*vec3(.2,.2,.2) , 1);
}      