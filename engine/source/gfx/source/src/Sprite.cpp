#include "Sprite.hpp"
#include "Sprite_atlas.hpp"
#include "string_id.hpp"
#include "Rect.hpp"
#include "vec3.hpp"
#include "vec2.hpp"
#include "Graphics_manager.hpp"

#include <stdint.h>
#include <string>
#include <iostream>
#include <fstream>



gfx::Sprite::Sprite(const Sprite_atlas *patlas, const std::uint8_t layer) : 
        m_is_active(true), m_size_in_wld_units(), m_patlas(patlas), m_layer(layer) {}


void gfx::Sprite::update_uv(const string_id sprite_id)
{
    const Atlas_image & image = m_patlas->get_image_data(sprite_id);
	
    m_size_in_wld_units.x = image.m_width / gfx::g_graphics_mgr.get_pixels_per_wld_unit();
    m_size_in_wld_units.y = image.m_height / gfx::g_graphics_mgr.get_pixels_per_wld_unit();

	m_vertices_uv[0] = math::vec2(image.m_texture_coordinates.x,
                                  image.m_texture_coordinates.y + image.m_texture_coordinates.height);

	m_vertices_uv[1] = math::vec2(image.m_texture_coordinates.x + image.m_texture_coordinates.width,
                                  image.m_texture_coordinates.y + image.m_texture_coordinates.height);

	m_vertices_uv[2] = math::vec2(image.m_texture_coordinates.x + image.m_texture_coordinates.width,
                                  image.m_texture_coordinates.y);

	m_vertices_uv[3] =   m_vertices_uv[2];
	m_vertices_uv[4] = math::vec2(image.m_texture_coordinates.x, image.m_texture_coordinates.y);
	m_vertices_uv[5] =   m_vertices_uv[0];
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