#include "Sprite.hpp"
#include "Sprite_atlas.hpp"
#include "Rect.hpp"
#include "vec3.hpp"
#include "vec2.hpp"

#include <stdint.h>
#include <string>
#include <iostream>
#include <fstream>



gfx::Sprite::Sprite(const Sprite_atlas *patlas, const std::uint8_t layer, const float pixels_per_unit) : m_patlas(patlas), m_layer(layer) ,m_pixels_per_unit(pixels_per_unit)
{
	
	//TODO: REMEBER TO SCALE THE POSITION DATA ACCORDING TO THE SPRITE_WIDH, SPRITE_HEIGHT AND PIXELS_PER_UNIT!!
	
	math::Rect rect;
	float sprite_width, sprite_height;
	m_patlas->get_text_coord(0, &rect, &sprite_width, &sprite_height);
	//tgs::Rect rect = m_atlas->get_text_coord(0);

	m_scale.x = sprite_width / m_pixels_per_unit;
	m_scale.y = sprite_height / m_pixels_per_unit;

	m_vertices_pos[0] = math::vec3( -(m_scale.x / 2.0f), -(m_scale.y / 2.0f), 0.0f);
	m_vertices_pos[1] = math::vec3(m_scale.x / 2.0f  , -(m_scale.y / 2.0f), 0.0f);
	m_vertices_pos[2] = math::vec3(m_scale.x / 2.0f, m_scale.y / 2.0f, 0.0f);
	m_vertices_pos[3] = math::vec3(m_scale.x / 2.0f, m_scale.y / 2.0f, 0.0f);
	m_vertices_pos[4] = math::vec3( -(m_scale.x / 2.0f), m_scale.y / 2.0f, 0.0f);
	m_vertices_pos[5] = math::vec3(-(m_scale.x / 2.0f), -(m_scale.y / 2.0f), 0.0f);


	m_vertices_uv[0] = math::vec2(rect.x, rect.y + rect.height); //uv1
	m_vertices_uv[1] = math::vec2(rect.x + rect.width, rect.y + rect.height); //uv2
	m_vertices_uv[2] = math::vec2(rect.x + rect.width, rect.y); //uv3

	m_vertices_uv[3] = math::vec2(rect.x + rect.width, rect.y); //uv3
	m_vertices_uv[4] = math::vec2(rect.x, rect.y); // uv0
	m_vertices_uv[5] = math::vec2(rect.x, rect.y + rect.height); //uv1

}


void gfx::Sprite::update_uv(const int sprite_no)
{
	math::Rect rect;
	float sprite_width, sprite_height;
	m_patlas->get_text_coord(sprite_no, &rect, &sprite_width, &sprite_height);
	//tgs::Rect rect = m_atlas->get_text_coord(0);
	if ( (m_scale.x != sprite_width / m_pixels_per_unit) || (m_scale.y != sprite_height / m_pixels_per_unit) ) {
		m_scale.x = sprite_width / m_pixels_per_unit;
		m_scale.y = sprite_height / m_pixels_per_unit;
	}

	m_vertices_uv[0] = math::vec2(rect.x, rect.y + rect.height); //uv1
	m_vertices_uv[1] = math::vec2(rect.x + rect.width, rect.y + rect.height); //uv2
	m_vertices_uv[2] = math::vec2(rect.x + rect.width, rect.y); //uv3

	m_vertices_uv[3] =   m_vertices_uv[2]; //uv3
	m_vertices_uv[4] = math::vec2(rect.x, rect.y); // uv0
	m_vertices_uv[5] =   m_vertices_uv[0]; //uv1	

	//std::cout << "updating uv coordinate to " << sprite_no << std::endl;
}

void gfx::Sprite::update_pos(const math::vec3 & pos, const bool facing_left)
{
	m_scale.x = (facing_left) ? (m_scale.x) : (-m_scale.x);

	m_vertices_pos[0] = math::vec3(-(m_scale.x / 2.0f), -(m_scale.y / 2.0f), 0.0f) + pos;
	m_vertices_pos[1] = math::vec3(m_scale.x / 2.0f, -(m_scale.y / 2.0f), 0.0f) + pos;
	m_vertices_pos[2] = math::vec3(m_scale.x / 2.0f, m_scale.y / 2.0f, 0.0f) + pos;
	m_vertices_pos[3] = m_vertices_pos[2];
	m_vertices_pos[4] = math::vec3(-(m_scale.x / 2.0f), m_scale.y / 2.0f, 0.0f) + pos;
	m_vertices_pos[5] = m_vertices_pos[0];

	m_scale.x = (facing_left) ? (m_scale.x) : (-m_scale.x);
}


const gfx::Sprite_atlas *gfx::Sprite::get_atlas() const 
{
	return m_patlas;
}

/*
void gfx::Sprite::update_animation(const float dt, Animator_controller *pcontroller)
{
	if (pcontroller) { 
		Animator_state & curr_state = pcontroller->get_current_state();
		if (curr_state.changed_animation_frame()) {
			update_uv(curr_state.get_curr_anim_frame());
		}
	}
	else {
#ifndef NDEBUG
		std::cerr << "WARNING: " << __FUNCTION__ << " This sprite is not animated" << std::endl;
#endif // !NDEBUG
	}
}*/