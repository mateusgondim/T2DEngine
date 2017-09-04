#version 430 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_uv;

out vec4 f_pos;
out vec2 f_uv;

uniform mat4 V;
uniform mat4 P;

void main() 
{
	gl_Position = P * V * vec4(v_pos, 1.0f);
	f_pos = P * V * vec4(v_pos, 1.0f);
	f_uv  = vec2(v_uv.x, 1.0f - v_uv.y);
}