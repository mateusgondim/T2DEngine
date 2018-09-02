#include <iostream>
#include <set>
#include <cstddef>

#include "Tileset.hpp"
#include "Property.hpp"
#include "Tile.hpp"
#include "vec2.hpp"


//Calculate the texture coordinates u and v, using the tile coordinate
/* TODO: Take into consideration the first_gid and the number of tiles

*/
void Tileset::get_text_coord(const int set_coord, math::vec2 & uv0, math::vec2 & uv1, math::vec2 & uv2, math::vec2 & uv3) const
{
	int origin_x = m_margin;
	int origin_y = m_set_height - m_margin;
	int coord = set_coord - m_first_gid + 1; // adjust to the interval [1, tile_count]

	int row = (coord - 1) / m_columns;
	int column = (coord - 1) % m_columns;



	int tile_y_top_left = origin_y - row    * (m_tile_height + m_spacing);//spacing only if row > 0
	int tile_x_top_left = origin_x + column * (m_tile_width + m_spacing);

	int tile_y_top_right = tile_y_top_left;
	int tile_x_top_right = tile_x_top_left + m_tile_width;

	int tile_y_bottom_left = tile_y_top_left - m_tile_height;
	int tile_x_bottom_left = tile_x_top_left;

	int tile_y_bottom_right = tile_y_bottom_left;
	int tile_x_bottom_right = tile_x_top_right;

	//normalize coordinates
	uv0.x = (float)tile_x_top_left / (float)m_set_width; // u for top left
	uv0.y = (float)tile_y_top_left / (float)m_set_height; // v for top left

	uv1.x = (float)tile_x_bottom_left / (float)m_set_width;//u for bottom left
	uv1.y = (float)tile_y_bottom_left / (float)m_set_height;//v for bottom left

	uv2.x = (float)tile_x_bottom_right / (float)m_set_width; //u for bottom right
	uv2.y = (float)tile_y_bottom_right / (float)m_set_height; //v for bottom right

	uv3.x = (float)tile_x_top_right / (float)m_set_width; // u for top right
	uv3.y = (float)tile_y_top_right / (float)m_set_height; // v for top right
}
 
//SHOULD IT DESTROY THE ASSOCIATED TEXTURE???????????????????????????????????????????????????????
Tileset::~Tileset() 
{
	delete[] m_ptiles;
}

std::ostream & operator<<(std::ostream & os, const Tileset & tileset)
{
	os << "Tileset data " << std::endl;
	os << "<tileset" << " firstgid=" << tileset.m_first_gid << " tilewidth=" << tileset.m_tile_width << " tileheight=" << tileset.m_tile_height << " margin=" << tileset.m_margin << " spacing=" << tileset.m_spacing
		<< " tilecount=" << tileset.m_tile_count << " columns=" << tileset.m_columns << " >" << std::endl;
	os << " <image " << "source= " << tileset.m_texture_file_path << " width=" << tileset.m_set_width << " height=" << tileset.m_set_height << " />" << std::endl;
	for (size_t i = 0; i != tileset.m_tile_count; ++i) {
		const Tile *ptile = tileset.m_ptiles + i;
		os << " <tile id=\"" << tileset.m_first_gid + i  << "\">";
		if (ptile->m_properties.cbegin() != ptile->m_properties.cend()) {
			os << "  <properties>" << std::endl;
			for (std::set<Property>::iterator it = ptile->m_properties.cbegin(); it != ptile->m_properties.cend(); ++it) {
				os << "\t" << *it << std::endl;
			}
			os << "  </properties>" << std::endl;
		}
		os << " </tile>" << std::endl;
	}
	os << "</tileset>" << std::endl;

	return os;
}