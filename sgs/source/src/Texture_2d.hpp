#ifndef _TEXTURE_2D_HPP

#define _TEXTURE_2D_HPP	
#include <string>

/*Texture_2d: Class for storing a opengl 2d texture, it keeps
 * a reference count , so it knows how many texture objects
 * refer to the same texture id, when the count goes to zero, the id is destroyed.
 */

namespace tgs {
	class Texture_2d {
	public:
						Texture_2d() : m_pref_count(new int(1)), m_id(0) {} // id = 0  represents that this texture_2d is not associated with any opengl texture 
						Texture_2d(const std::string & image_path);
						
						Texture_2d(const Texture_2d & t);
		Texture_2d  &   operator=(const Texture_2d & rhs);
						~Texture_2d();

		std::string		get_image_path()   const { return m_image_path; }
		void			use(const unsigned unit = 0) const ;
		
	private:
		void			load_texture(const std::string & image_path);
		void			delete_texture();

		int				*m_pref_count;
		unsigned		m_id;
		int				m_width;
		int				m_height;
		std::string		m_image_path;
	};
}

#endif // !_TEXTURE_2D_HPP
