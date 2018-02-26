#include "Camera_2d.hpp"
#include "vec3.hpp"
#include "matrix_transform.hpp"
#include <cmath>

gfx::Camera_2d::Camera_2d(float tile_width, float tile_height, float screen_tile_width, float screen_tile_height) :
	m_tile_width(tile_width), m_tile_height(tile_height), m_screen_tile_width(screen_tile_width), m_screen_tile_height(screen_tile_height) 
{
	float width = m_screen_tile_width * m_tile_width; // the width of the screen in world units
	float height = m_screen_tile_height * m_tile_height; // the height of the screen in world units

	m_projection = cgm::ortho(-floor(width / (float)2), ceil(width / (float)2), -floor(height / (float)2), ceil(height  / (float)2), -1.0, 1.0f);
	m_transform.get_translation() = cgm::vec3(floor(width / (float)2), floor(height / (float)2), 0.0f);
	//m_position = cgm::vec3(-floor(width / (float)2) + map_origin.x, -floor(height / (float)2) + map_origin.y, 0.0f);
	//remeber that this position calculated above, is the "smallest" position, i.e, if the camera moves more to the left or up, it will see behond the maps border
}

void gfx::Camera_2d::follow(const cgm::vec3 & p)
{
	float width = m_screen_tile_width * m_tile_width; // the width of the screen in world units
	float t_x = p.x - m_transform.get_translation().x;
	m_transform.get_translation().x = (floor(width / (float)2) < m_transform.get_translation().x + t_x) ? (m_transform.get_translation().x + t_x) : (floor(width / (float)2));
	
}