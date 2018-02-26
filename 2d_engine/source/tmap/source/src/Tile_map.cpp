#include "Tile_map.hpp"

#include "Tile.hpp"
#include "Tileset.hpp"
#include "Rect.hpp"

//allocate the tile_map and assign the corresponding tileset coordinate for each tile
void Tile_map::setup_map(const std::vector<std::vector<std::vector<int>>> & layers)
{
	std::vector<std::vector<unsigned>> aux(m_height, std::vector<unsigned>(m_width));
	m_map = std::vector<std::vector<std::vector<unsigned>>>(m_layer_count, aux);

	for (int l = 0; l < m_layer_count; ++l) {
		for (int i = 0; i < m_height; ++i) {
			for (int j = 0; j < m_width; ++j) {
				m_map[l][i][j] = layers[l][i][j];
			}
		}
	}
}
//TODO: THROW EXCEPTION OR CHANGE TO TILE* AND RETURN NULLPTR
Tile Tile_map::get_tile(const unsigned id) const 
{
	for (auto iter = m_tilesets.begin(); iter != m_tilesets.end(); ++iter) {
		if (iter->is_inside_set(id)) {
			return iter->get_tile(id);
		}
	}
	return Tile();
}

//TAKE INTO  CONSIDERATION THE PIXELS PER_WORLD UNIT
Rect Tile_map::tile_wld_space_bounds(const unsigned row, const unsigned column) const
{	
	cgm::vec3 map_space_coord(column * m_tile_width / m_pixels_per_word_unit, (m_height * m_tile_height / m_pixels_per_word_unit) - (row * m_tile_height / m_pixels_per_word_unit), 0.0f);
	cgm::vec3 wld_space_coord = map_space_coord + m_position;

	Rect rect;
	rect.width   =  m_tile_width / m_pixels_per_word_unit;
	rect.height  =  m_tile_height / m_pixels_per_word_unit;
	rect.x       =  wld_space_coord.x;
	rect.y       =  wld_space_coord.y;

	return rect;
}

//get the row and column from a point in world space
std::pair<float, float> Tile_map::wld_to_tile_space(const cgm::vec3 & pos) const 
{
	float rows = (m_height * m_tile_height / m_pixels_per_word_unit) - ((pos.y - m_position.y) / (m_tile_height / m_pixels_per_word_unit));
	float columns =  (pos.x - m_position.x) / (m_tile_width / m_pixels_per_word_unit);

	return{ rows, columns };
}

std::ostream & print_tile_map(std::ostream & os, const Tile_map & tile_map) 
{
	os << "Tile map data " << std::endl;
	os << "<map " << "width=" << tile_map.m_width << " height=" << tile_map.m_height << " tilewidth=" << tile_map.m_tile_width << " tileheight=" << tile_map.m_tile_height << " >" << std::endl;

	for (int i = 0; i < tile_map.m_tilesets.size(); ++i) {
		//print tilesets
		print_tileset(std::cout, tile_map.m_tilesets[i]);
	}

	//layers
	for (int l = 0; l < tile_map.m_layer_count; ++l) {
		os << "<layer>" << std::endl;
		os << "  <data>" << std::endl;
		for (int i = 0; i < tile_map.m_height; ++i) {
			for (int j = 0; j < tile_map.m_width; ++j) {
				os << tile_map.m_map[l][i][j] << ",";
			}
			os << std::endl;
		}
		os << "</data>" << std::endl;
		os << "  </layer>" << std::endl;
	}
	return os;
}