#version 430 core

layout (location = 0) in vec3 v_pos;

//Used for Debugging, displays the tile grid

uniform mat4 V;
uniform mat4 P;

void main() 
{
	gl_Position = P * V * vec4(v_pos, 1.0f);
}