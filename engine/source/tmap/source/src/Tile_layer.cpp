#include "Tile_layer.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

Tile_layer::Tile_layer() : 
        m_pname(nullptr), m_x(0.0f), m_y(0.0f), m_opacity(1.0f), m_is_visible(true), m_x_offset_pixels(0), m_y_offset_pixels(0) {}

Tile_layer::~Tile_layer() 
{
        if (m_pname) {
                free(m_pname);
        }
}

void Tile_layer::set_name(const char *str) 
{
        if (m_pname) {
                free(m_pname);
        }
        m_pname = strdup(str);
}

std::ostream & operator<<(std::ostream & os, const Tile_layer & layer) 
{
	os << "< layer name=\"" << layer.m_pname << "\" width= \"" 
	   << layer.m_width_in_tiles << "\" height= \"" << layer.m_height_in_tiles << "\" >" << std::endl;
	os << "\t <data >" << std::endl;
	for (std::vector<uint32_t>::size_type i = 0; i != layer.m_height_in_tiles; ++i) {
		for (std::vector<uint32_t>::size_type j = 0; j != layer.m_width_in_tiles; ++j) {
			os << layer.m_data[i * layer.m_width_in_tiles + j] << ",";
		}
		os << std::endl;
	}
	os << "\t </data>" << std::endl;
	os << "</layer>";

	return os;
}