#ifndef _TILE_HPP

#define _TILE_HPP

struct Tile {
	Tile() = default;
	Tile(const int width, const int height) : m_width(width), m_height(height) {}
	//width and height in pixels
	int         m_width           =  16; 
	int         m_height		  =  16;
	unsigned    m_tileset_coord   =   0;
	//TODO: sound, is_obstacle, is_stairs etc
	bool	    m_is_obstacle     = false;
};

#endif // !_TILE_HPP

