#version 400 core

in vec4 f_pos;
in vec4 f_color;
in vec2 f_uv;

out vec4 color;

uniform sampler2D ui_atlas;

void main() 
{
	vec4 tex_color = texture(ui_atlas, f_uv);
	if ( (tex_color.a == 0.0f) ) { //transparent color
		discard;
	}
	else {
		color = tex_color;
	}
}