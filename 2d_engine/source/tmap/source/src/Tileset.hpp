#ifndef _TILESET_HPP

#define _TILESET_HPP

#include <iostream>
#include <string>
#include <vector>

#include "Tile.hpp"
#include "Texture_2d.hpp"
/* -------------------------Tileset---------------------------------------
 * Data structure to store a vector of tiles contained in a texture image,
 * every tile has a unique identifier, the first tile in the set has the id equal to
 * first_global_id, and the last tile has an id equal to first_global_id + tile_count -1.
 * To be able to access the tiles stored in the texture, it is also stored the parameters of
 * the texture image, such as the space beethwen tiles in pixels, the number of columns, the 
 * size of the image in pixels and so forth.
 *-------------------------------------------------------------------------
 */

//TODO: ADD THE DESCTURCTOR TO DEALLOCATE TILE*
//TODO: PUT Load_tileset has a friend or add another parameter on the constructor to take the pointer

namespace cgm { struct vec2; }

class Tileset {
friend std::ostream & print_tileset(std::ostream & os, const Tileset & tileset);
public:
	Tileset() = default;
	Tileset(Tile * ptiles, const int  first_gid, const std::string & image_path, const int tile_width, const int tile_height, const int margin, const int spacing, const int tile_count, const int columns, const int set_width, const int set_height) 
		: m_ptiles(ptiles), m_first_gid(first_gid), m_texture(image_path), m_tile_width(tile_width), m_tile_height(tile_height), m_margin(margin), m_spacing(spacing), 
		m_tile_count(tile_count), m_columns(columns), m_set_width(set_width), m_set_height(set_height) {}
	
	void get_text_coord(const int set_coord, cgm::vec2 & uv0, cgm::vec2 & uv1, cgm::vec2 & uv2, cgm::vec2 & uv3) const;
	const gfx::Texture_2d & get_texture() const { return m_texture; }
	bool is_inside_set(const unsigned id) const{ return ((id >= m_first_gid) && (id <= m_first_gid + m_tile_count - 1)) ? (true) : (false);} // check if this tile is inside the tileset image
	Tile get_tile(const unsigned id)      const { return m_ptiles[id - m_first_gid]; } //assumes id is inside set
private:
	int			     m_first_gid;  // the id of the first tile in the tileset
	gfx::Texture_2d  m_texture;
	int			     m_tile_width;  // witdh of the tiles in pixel
	int			     m_tile_height; // height of the tiles in pixel
	int			     m_margin; 
	int			     m_spacing;
	int		         m_tile_count; // number of tiles stored in the image
	int              m_columns;
	int			     m_set_width;
	int			     m_set_height;
	
	Tile		     *m_ptiles; //pointer to array of tiles
};

std::ostream & print_tileset(std::ostream & os, const Tileset & tileset);

#endif // !_TILESET_HPP

