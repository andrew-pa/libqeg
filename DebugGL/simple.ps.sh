#version 400 core
layout(location=0) out vec4 color;
in vec3 posW;
in vec3 normW;
in vec2 texc;
void main()
{
	color = vec4(normW + vec3(texc, 0),1);
}      