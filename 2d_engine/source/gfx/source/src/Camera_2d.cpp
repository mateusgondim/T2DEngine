#include "Camera_2d.hpp"
#include "vec3.hpp"
#include "matrix_transform.hpp"
#include <cmath>
#include <iostream>

gfx::Camera_2d::Camera_2d(float tile_width_wld, float tile_height_wld, float tiles_per_screen_width, float tiles_per_screen_height, int map_width, int map_height, const math::vec2 & map_wld_origin) 
{
	init(tile_width_wld, tile_height_wld, tiles_per_screen_width, tiles_per_screen_height, map_width, map_height, map_wld_origin);
	/*
	float width = m_tiles_per_screen_width * m_tile_width_wld; // the width of the screen in world units
	float height = m_tiles_per_screen_height * m_tile_height_wld; // the height of the screen in world units

	//set the screen boundaries 
	m_screen_p_min.x = -floor(width / 2.0f);
	m_screen_p_min.y = -floor(height / 2.0f);

	m_screen_p_max.x = ceil(width / 2.0f);
	m_screen_p_max.y = ceil(height / 2.0f);


	//set the projection matrix
	m_projection = math::ortho(m_screen_p_min.x, m_screen_p_max.x, m_screen_p_min.y, m_screen_p_max.y, -1.0, 1.0f);
	
	//set the starting screen's rectangle position
	math::vec2 screen_wld_pos(floor(width / 2.0f), floor(height /2.0f));
	
	//translate to the position
	m_transform.get_translation() = math::vec3(screen_wld_pos.x, screen_wld_pos.y, 0.0f);

	//store the tile map world Rect coordinates 
	m_map_wld_p_min = map_wld_origin;
	m_map_wld_p_max = math::vec2(map_wld_origin.x + map_width * tile_width_wld, map_wld_origin.y + map_height * m_tile_height_wld);
	*/
}

void gfx::Camera_2d::init(float tile_width_wld, float tile_height_wld, float tiles_per_screen_width, float tiles_per_screen_height, int map_width, int map_height, const math::vec2 & map_wld_origin) 
{
	m_tile_width_wld		   =  tile_width_wld;
	m_tile_height_wld		   =  tile_height_wld;
	
	set_screen_dim(tiles_per_screen_width, tiles_per_screen_height);

	//set the starting screen's rectangle position
	math::vec2 screen_wld_pos(floor(m_tiles_per_screen_width * m_tile_width_wld / 2.0f), floor(m_tiles_per_screen_height * m_tile_height_wld / 2.0f));

	//translate to the position
	m_transform.get_translation() = math::vec3(screen_wld_pos.x, screen_wld_pos.y, 0.0f);

	//store the tile map world Rect coordinates 
	m_map_wld_p_min = map_wld_origin;
	m_map_wld_p_max = math::vec2(map_wld_origin.x + map_width * tile_width_wld, map_wld_origin.y + map_height * m_tile_height_wld);
}

void gfx::Camera_2d::set_screen_dim(const float tiles_per_screen_width, const float tiles_per_screen_height) 
{
	m_tiles_per_screen_width = tiles_per_screen_width;
	m_tiles_per_screen_height = tiles_per_screen_height;

	float width = m_tiles_per_screen_width * m_tile_width_wld; // the width of the screen in world units
	float height = m_tiles_per_screen_height * m_tile_height_wld; // the height of the screen in world units

																  //set the screen boundaries 
	m_screen_p_min.x = -floor(width / 2.0f);
	m_screen_p_min.y = -floor(height / 2.0f);

	m_screen_p_max.x = ceil(width / 2.0f);
	m_screen_p_max.y = ceil(height / 2.0f);

	//set the projection matrix
	m_projection = math::ortho(m_screen_p_min.x, m_screen_p_max.x, m_screen_p_min.y, m_screen_p_max.y, -1.0, 1.0f);

}

void gfx::Camera_2d::follow(const math::vec3 & p)
{
	//calculate the translation vector to follow p 
	math::vec3 translation = p - m_transform.get_translation();
	
	// calculate the new camera position
	math::vec3 new_cam_pos = m_transform.get_translation() + translation;
	

	//calculate the map's Rect in screen space
	math::vec2 map_screen_p_min = math::vec2(m_map_wld_p_min.x - new_cam_pos.x, m_map_wld_p_min.y - new_cam_pos.y);
	math::vec2 map_screen_p_max = math::vec2(m_map_wld_p_max.x - new_cam_pos.x, m_map_wld_p_max.y - new_cam_pos.y);

	// check if the horizontal translation will move the screen's rect beyond the map
	if (!(m_screen_p_max.x <= map_screen_p_max.x  && m_screen_p_min.x >= map_screen_p_min.x )) {
		if (translation.x > 0) { // scrool just enough to see the map's horizontal border
			new_cam_pos.x += map_screen_p_max.x - m_screen_p_max.x;
		}
		else {
			new_cam_pos.x += map_screen_p_min.x - m_screen_p_min.x;
		}
	}
	
	// check if the vertical translation will move the screen's rect beyond the map
	if (!(m_screen_p_min.y >= map_screen_p_min.y && m_screen_p_max.y <= map_screen_p_max.y )) {
		if (translation.y > 0) { // scrool just enought to see the map's vertical border
			new_cam_pos.y += map_screen_p_max.y - m_screen_p_max.y;
		}
		else {
			new_cam_pos.y += map_screen_p_min.y - m_screen_p_min.y;
		}
	}

	m_transform.get_translation() = new_cam_pos;
}

// check if the object on position p with width = 'width' and height = 'height' is outside the camera's view
bool gfx::Camera_2d::is_off_camera(const math::vec3 & p, const float width, const float height) const
{
	math::vec3 p_screen_space = p - m_transform.get_translation();
	
	if ( (p_screen_space.x + width / 2.0f < m_screen_p_min.x) || (p_screen_space.x - width / 2.0f > m_screen_p_max.x) ) {
		return true;
	}
	if ((p_screen_space.y + height / 2.0f < m_screen_p_min.y) || (p_screen_space.y - height / 2.0f > m_screen_p_max.y)) {
		return true;
	}
	return false;
}