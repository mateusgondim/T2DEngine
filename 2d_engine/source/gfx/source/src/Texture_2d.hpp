#ifndef _TEXTURE_2D_HPP

#define _TEXTURE_2D_HPP	
#include <string>
#include <cstdint>
#include "string_id.hpp"

/*Texture_2d: Class for storing a opengl 2d texture, it keeps
 * a reference count , so it knows how many texture objects
 * refer to the same texture id, when the count goes to zero, the id is destroyed.
 */

namespace gfx {
	class Texture_2d final {
	public:
						Texture_2d() : m_pref_count(new int32_t(1)), m_texture_id(0) {} // texture_id = 0  represents that this texture_2d is not associated with any opengl texture 
						Texture_2d(const std::string & image_path);
						
						Texture_2d(const Texture_2d & t);
						Texture_2d  &   operator=(const Texture_2d & rhs);
						
						~Texture_2d();
		
		string_id		get_string_id()   const { return m_file_name_id; }
		void			use(const uint32_t unit = 0) const ;
		int32_t    get_width() const { return m_width; }
		int32_t    get_height() const { return m_height; }
	private:
		void			load_texture(const std::string & image_path);
		void			delete_texture();

		uint32_t	m_texture_id; 
		int32_t		m_width;
		int32_t		m_height;
		string_id	m_file_name_id;

		int32_t *m_pref_count;
	};
}

#endif // !_TEXTURE_2D_HPP
