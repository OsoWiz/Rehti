
R""(
#version 450 core

vec4[] positions = vec4[](vec4(1.0, 0.0, 0.0, 1.0), vec4(-1.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, -1.0, 1.0));

layout(location = 0) out vec3 fragPos;

void main()
{
	gl_Position = positions[gl_VertexIndex];
	fragPos = positions[gl_VertexIndex].xyz;
}

)"";