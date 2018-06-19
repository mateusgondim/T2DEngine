#version 400 core

in vec4 f_pos;
in vec4 f_color;
in vec2 f_uv;

out vec4 color;

uniform sampler2D tileset1;

void main() 
{
	color = texture(tileset1, f_uv);
}