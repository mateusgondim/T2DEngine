#ifndef _SPRITE_ATLAS_HPP
#define _SPRITE_ATLAS_HPP

#include "Texture_2d.hpp"
#include "vec2.hpp"
#include "Rect.hpp"
#include <string>
#include <vector>
/*Sprite_atlas: this class stores all the data associated with
 * a sprite atlas necessary for rendering it, including the texture_2d  
 * object and, a vector of rect objects containg the UV coordinates for
 * all the sprites in the atlas
 */
namespace gfx {
	class Sprite_atlas {
	public:
		Sprite_atlas(const std::string & file_name, const float pixels_per_unit = 16.0f);
		
		unsigned            get_pixels_per_unit()  const { return m_pixels_per_unit; }
		Texture_2d          get_texture()		   const { return m_texture; }
		std::string			get_file_path()		   const { return m_file_path; }
		void get_text_coord(const int sprite_no, math::Rect * prect, float * px_width, float *px_height) const;
	private:
		std::string			m_file_path;       // atlas data file path
		float				m_pixels_per_unit; // number of pixels per game world's unit
		Texture_2d			m_texture;

		math::vec2			m_image_size; // width and height of this sprite in pixels
		std::vector<math::Rect>	m_vrec;		  // pixel coordinates of each sprite on the texture atlas

														  //TODO: SCALE THE VERTICES IN THE POSITION VECTOR WHEN CHANGING THE CURRENT VERTEX UV COORDINATES
														  //std::vector<unsigned> m_elements
	};
}

#endif // !_SPRITE_ATLAS_HPP
