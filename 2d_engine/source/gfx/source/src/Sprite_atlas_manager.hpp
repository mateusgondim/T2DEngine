#ifndef _SPRITE_ATLAS_MANAGER_HPP
#define _SPRITE_ATLAS_MANAGER_HPP

//TODO: Pixels_per_unit is a global variable tha should be the same for all the sprites in the games, find a way to maintain the consistency, maybe a variable in the "Engine" class...

#include "Resource_manager.hpp"
#include "string_id.hpp"

namespace rms { class Resource; }
namespace gfx { class Texture_2d_manager; }

namespace gfx {
	class Sprite_atlas_manager : public rms::Resource_manager {
	public:
		Sprite_atlas_manager();
		rms::Resource *load(const char *name, const char *file_path, Texture_2d_manager *texture_manager);
		
		void          unload(const char *name) override;
		void	      unload(string_id  id) override;
		void		  unload_all()	override;
		rms::Resource *create_impl(const char *name) override;
	};
}

#endif // !_SPRITE_ATLAS_MANAGER_HPP
