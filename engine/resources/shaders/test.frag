#version 450 core

layout(location = 0) in vec3 fragPos;
layout(location = 0) out vec4 outColor;

void main()
{
	vec3 color = clamp(abs(fragPos), 0.f, 1.f);
	outColor = vec4(color, 1.0);
}