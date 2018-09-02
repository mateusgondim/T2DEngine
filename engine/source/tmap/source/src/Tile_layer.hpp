#ifndef _TILE_LAYER_HPP
#define _TILE_LAYER_HPP

#include <vector>
#include <stdint.h>
#include <iostream>

class Tile_layer {
friend class Tile_map;
friend std::ostream & operator<<(std::ostream & os, const Tile_layer & layer);
public:
        Tile_layer();

	~Tile_layer();

        void            set_name(const char *str);

	uint32_t        get_tile_id(uint32_t row, uint32_t column) const;
        const char *    get_name() const;
        int             get_width_in_tiles() const;
        int             get_height_in_tiles() const;
        const std::vector<uint32_t> &           get_data() const;
private:
        char *		m_pname;
        float           m_x;
        float           m_y;
        int             m_width_in_tiles;
	int             m_height_in_tiles;
        float           m_opacity;
	bool            m_is_visible;
        int	        m_x_offset_pixels;
        int             m_y_offset_pixels;
        std::vector<uint32_t>   m_data;
};

std::ostream & operator<<(std::ostream & os, const Tile_layer & layer);

inline uint32_t Tile_layer::get_tile_id(uint32_t row, uint32_t column) const 
{
        return m_data[row * m_width_in_tiles + column];
}

inline const char * Tile_layer::get_name() const 
{
        return m_pname;
}

inline int Tile_layer::get_width_in_tiles() const 
{
        return m_width_in_tiles;
}

inline int Tile_layer::get_height_in_tiles() const 
{
        return m_height_in_tiles;
}

inline const std::vector<uint32_t> & Tile_layer::get_data() const 
{
        return m_data;
}

#endif // !_TILE_LAYER_HPP
