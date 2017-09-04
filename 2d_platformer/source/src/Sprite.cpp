#include "Sprite.hpp"
#include "Sprite_atlas_manager.hpp"
#include "Rect.hpp"
#include "vec2.hpp"
#include "Rect.hpp"
#include <iostream>
#include <fstream>



tgs::Sprite::Sprite(const std::string & atlas_data_file, const float pixels_per_unit) : m_pixels_per_unit(pixels_per_unit) 
{
	m_atlas = Sprite_atlas_manager::instance().load_atlas(atlas_data_file, pixels_per_unit);
	//TODO: REMEBER TO SCALE THE POSITION DATA ACCORDING TO THE SPRITE_WIDH, SPRITE_HEIGHT AND PIXELS_PER_UNIT!!
	
	tgs::Rect rect;
	float sprite_width, sprite_height;
	m_atlas->get_text_coord(0, &rect, &sprite_width, &sprite_height);
	//tgs::Rect rect = m_atlas->get_text_coord(0);

	m_scale.x = sprite_width / m_pixels_per_unit;
	m_scale.y = sprite_height / m_pixels_per_unit;

	m_vertices_pos.push_back(cgm::vec3( -(m_scale.x / 2.0f), -(m_scale.y / 2.0f), 0.0f));
	m_vertices_pos.push_back(cgm::vec3(m_scale.x / 2.0f  , -(m_scale.y / 2.0f), 0.0f));
	m_vertices_pos.push_back(cgm::vec3(m_scale.x / 2.0f, m_scale.y / 2.0f, 0.0f));
	m_vertices_pos.push_back(cgm::vec3(m_scale.x / 2.0f, m_scale.y / 2.0f, 0.0f));
	m_vertices_pos.push_back(cgm::vec3( -(m_scale.x / 2.0f), m_scale.y / 2.0f, 0.0f));
	m_vertices_pos.push_back(cgm::vec3(-(m_scale.x / 2.0f), -(m_scale.y / 2.0f), 0.0f));


	m_vertices_uv.push_back(cgm::vec2(rect.x, rect.y + rect.height)); //uv1
	m_vertices_uv.push_back(cgm::vec2(rect.x + rect.width, rect.y + rect.height)); //uv2
	m_vertices_uv.push_back(cgm::vec2(rect.x + rect.width, rect.y)); //uv3

	m_vertices_uv.push_back(cgm::vec2(rect.x + rect.width, rect.y)); //uv3
	m_vertices_uv.push_back(cgm::vec2(rect.x, rect.y)); // uv0
	m_vertices_uv.push_back(cgm::vec2(rect.x, rect.y + rect.height)); //uv1
}


void tgs::Sprite::update_uv(const int sprite_no) 
{
	tgs::Rect rect;
	float sprite_width, sprite_height;
	m_atlas->get_text_coord(sprite_no, &rect, &sprite_width, &sprite_height);
	//tgs::Rect rect = m_atlas->get_text_coord(0);
	if ( (m_scale.x != sprite_width / m_pixels_per_unit) || (m_scale.y != sprite_height / m_pixels_per_unit) ) {
		m_scale.x = sprite_width / m_pixels_per_unit;
		m_scale.y = sprite_height / m_pixels_per_unit;
	}

	m_vertices_uv[0] =   cgm::vec2(rect.x, rect.y + rect.height); //uv1
	m_vertices_uv[1] =   cgm::vec2(rect.x + rect.width, rect.y + rect.height); //uv2
	m_vertices_uv[2] =   cgm::vec2(rect.x + rect.width, rect.y); //uv3

	m_vertices_uv[3] =   m_vertices_uv[2]; //uv3
	m_vertices_uv[4] =   cgm::vec2(rect.x, rect.y); // uv0
	m_vertices_uv[5] =   m_vertices_uv[0]; //uv1	

	std::cout << "updating uv coordinate to " << sprite_no << std::endl;
}

void tgs::Sprite::update_pos(const cgm::vec3 & pos, const bool facing_left) 
{
	m_scale.x = (facing_left) ? (m_scale.x) : (-m_scale.x);

	m_vertices_pos[0] = cgm::vec3(-(m_scale.x / 2.0f), -(m_scale.y / 2.0f), 0.0f) + pos;
	m_vertices_pos[1] = cgm::vec3(m_scale.x / 2.0f, -(m_scale.y / 2.0f), 0.0f) + pos;
	m_vertices_pos[2] = cgm::vec3(m_scale.x / 2.0f, m_scale.y / 2.0f, 0.0f) + pos;
	m_vertices_pos[3] = m_vertices_pos[2];
	m_vertices_pos[4] = cgm::vec3(-(m_scale.x / 2.0f), m_scale.y / 2.0f, 0.0f) + pos;
	m_vertices_pos[5] = m_vertices_pos[0];

	m_scale.x = (facing_left) ? (m_scale.x) : (-m_scale.x);
}