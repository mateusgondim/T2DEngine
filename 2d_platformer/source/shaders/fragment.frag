#version 430 core

in vec4 f_pos;
in vec2 f_uv;

out vec4 f_color;

uniform sampler2D tileset1;

void main() 
{
	f_color = texture(tileset1, f_uv);
}