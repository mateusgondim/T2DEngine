#ifndef _SPRITE_ATLAS_HPP
#define _SPRITE_ATLAS_HPP

#include "Resource.hpp"
#include "Sprite_atlas_manager.hpp"
#include "Rect.hpp"
#include "string_id.hpp"
#include <map>

/*Sprite_atlas: this class stores all the data associated with
 * a sprite atlas necessary for rendering it, including the texture_2d  
 * object and, a vector of rect objects containg the UV coordinates for
 * all the sprites in the atlas
 */
namespace rms { class Resource_manager; }
namespace gfx { class Texture_2d; class Texture_2d_manager; }

namespace gfx {

        // Struct containig data regarding a single image on the atlas
        struct Atlas_image {
                math::Rect      m_texture_coordinates;
                int             m_width; // in pixels
                int             m_height; // in pixels
        };

	class Sprite_atlas : public rms::Resource {
	friend rms::Resource *gfx::Sprite_atlas_manager::load(const char *name, const char *file_path,
                                                          gfx::Texture_2d_manager *texture_manager);
	public:
		Sprite_atlas(rms::Resource_manager *pcreator, const char *name);

		void load()    override;
		void unload()  override;
		size_t calculate_size() const override;

		void set_atlas_file_path(const char *file_path);
		const char *get_texture_file_path() const;

		//Sprite_atlas(const std::string & file_name, const string_id id);
		
		gfx::Texture_2d *get_texture();
		const Atlas_image & get_image_data(const string_id image_id) const;
		
	private:
		gfx::Texture_2d   *m_ptexture;				// size 16 bytes | alignment 4 bytes
	    char			  *m_atlas_file_path;
		char			  *m_texture_file_path;

        // Data for each sprite on the texture atlas
        std::map<string_id, Atlas_image>  m_images;

	  //TODO: SCALE THE VERTICES IN THE POSITION VECTOR WHEN CHANGING THE CURRENT VERTEX UV COORDINATES			
	};
}

#endif // !_SPRITE_ATLAS_HPP
