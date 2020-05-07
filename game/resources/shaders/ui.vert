#version 400 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec2 v_uv;

out vec4 f_pos;
out vec4 f_color;
out vec2 f_uv;


uniform mat4 P;

void main() 
{
	gl_Position = P * vec4(v_pos, 1.0f);
	f_pos = P * vec4(v_pos, 1.0f);
	f_uv  = vec2(v_uv.x, v_uv.y);
	f_color = v_color;
}