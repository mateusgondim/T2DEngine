#ifndef _TILE_MAP_HPP

#define _TILE_MAP_HPP

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <cassert>
#include <stdint.h>

#include "Tileset.hpp"
#include "Tile_layer.hpp"
#include "Object_group.hpp"
#include "vec3.hpp"
#include "vec2.hpp"


//------------------- TILE MAP---------------------------------------------------------//
/* This class defines a data structure to store a map of 2d tiles, it reads its data from a tmx file.
 *  -m_tilestes stores a vector of tilesets, each tileset, contains a array of tiles.
 *  -m_map is a 3d array of tiles, the third dimension is for each 2D layer in the tile map
 *
 * Tile_map space is a left-right top-bottom coordinate space
 */

struct Tile;
namespace math { struct Rect; }

class Tile_map {
friend std::ostream & print_tile_map(std::ostream & os, const Tile_map & map);
public:
        Tile_map(const std::string & tmx_file_path);
        //Tile_map() = default;
	
        /*Tile_map(const std::vector<Tileset> & tilesets, const std::vector<std::vector<std::vector<int>>> & layers ,const int layer_count, const int width, const int height, const int tile_width, const int tile_height) : m_pixels_per_word_unit(16)
	{
		m_tilesets      =  tilesets;
		m_layer_count   =  layer_count;
		m_width         =  width;
		m_height        =  height;
		m_tile_width    =  tile_width;
		m_tile_height   =  tile_height;
		//setup_map(layers);
	}*/
        ~Tile_map();

//	int             layer_count()  const { return m_layer_count; }
	int             width()        const { return m_width; }
	int             height()       const { return m_height; }
	int             tile_width()   const { return m_tile_width;}
	int             tile_height()  const { return m_tile_height; }
	const math::vec3 & get_background_color() const { return m_background_color; }
	const std::vector<Tileset*> & get_tilesets()  { return m_tilesets; }
	const Object_group *	get_object_group(const char *name) const;
        uint32_t        get_tile_id(uint32_t layer, uint32_t row, uint32_t column) const;
	const Tile *    get_tile(uint32_t id) const;
	float pixels_per_world_unit() const { return m_pixels_per_word_unit; }
	math::vec2	pixels_to_wld_coord(float x, float y) const;

	const math::vec3 & get_position() const { return m_position; }
    bool is_inside_map(const math::vec3 & p_wld) const;

	math::Rect      tile_wld_space_bounds(const unsigned row, const unsigned column) const;
	std::pair<float, float> wld_to_tile_space(const math::vec3 & pos) const;

	float           world_to_tile_displacement_x(float t_x) const { return t_x / (m_tile_width / m_pixels_per_word_unit) ; }
	float           world_to_tile_displacement_y(float t_y) const { return t_y / (m_tile_height / m_pixels_per_word_unit); }

	void            set_background_color(const math::vec3 & color) { m_background_color = color; }
private:
        void            parse_map_element_variables(const std::string & line);
        void            parse_tileset(const std::string & tmx_file_path, const std::string & first_line);

	std::vector<Tileset*> m_tilesets;
        std::vector<Tile_layer*> m_layers;
	std::vector<Object_group*> m_object_layer;
	
        //std::vector<std::vector<std::vector<unsigned>>>  m_map; // 3D array, i.e layers of 2d arrays	 
	
	//int             m_layer_count; // number of layers
	int             m_width;
	int             m_height;
	int             m_tile_width;
	int             m_tile_height;

	math::vec3      m_position;  // tile map origin, i.e, the coordinates of the top left tile in world space
	math::vec3      m_background_color;
	float           m_pixels_per_word_unit; // how many pixels are equal to 1 world unit, used in scaling
};


inline  uint32_t Tile_map::get_tile_id(uint32_t layer, uint32_t row, uint32_t column) const 
{
	//assert(row < m_height && "invalid row! outside map border");
	//assert(column < m_width && "invadlid column! outside maps border");

        return m_layers[layer]->get_tile_id(row, column);
	//return m_map[layer][row][column];
}

//checks if a point in world space is inside the tile map
inline bool Tile_map::is_inside_map(const math::vec3 & p_wld) const
{
        float tile_width_wld = m_tile_width / m_pixels_per_word_unit;
        float tile_height_wld = m_tile_height / m_pixels_per_word_unit;
        if ((p_wld.x < m_position.x + m_width * tile_width_wld) && (p_wld.x > m_position.x) && (p_wld.y < m_position.y + m_height * tile_height_wld) && (p_wld.y > m_position.y)) {
                return true;
        }
        else {
                return false;
        }
}

inline math::vec2 Tile_map::pixels_to_wld_coord(float x, float y) const
{
	return math::vec2(m_position.x + x / m_pixels_per_word_unit, m_position.y + (m_height * m_tile_height / m_pixels_per_word_unit - (y / m_pixels_per_word_unit)) );
}

std::ostream & print_tile_map(std::ostream & os, const Tile_map & tile_map);

#endif // !_TILE_MAP_HPP
