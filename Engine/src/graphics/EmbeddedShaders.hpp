#pragma once
#include <unordered_map>
#include <string>
namespace ShaderEmbedder {
std::unordered_map<std::string, std::string> shaders = {
	{"test.frag", R"(#version 450 core

layout(location = 0) in vec3 fragPos;
layout(location = 0) out vec4 outColor;

void main()
{
	vec3 color = clamp(abs(fragPos), 0.f, 1.f);
	outColor = vec4(color, 1.0);
})"},
	{"test.vert", R"(#version 450 core

vec4[] positions = vec4[](vec4(1.0, 0.0, 0.0, 1.0), vec4(-1.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, -1.0, 1.0));

layout(location = 0) out vec3 fragPos;

void main()
{
	gl_Position = positions[gl_VertexIndex];
	fragPos = positions[gl_VertexIndex].xyz;
})"},
};
}
