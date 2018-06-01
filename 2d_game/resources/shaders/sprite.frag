#version 430 core

in vec4 f_pos;
in vec4 f_color;
in vec2 f_uv;

out vec4 color;

uniform sampler2D tileset;

void main() 
{
	vec4 tex_color = texture(tileset, f_uv);
	if ( (tex_color.r == 0.0f) && (tex_color.g == 0.0f) && (tex_color.b == 0.0f) ) { //transparent color
		discard;
	}
	else {
		color = tex_color;
	}
}