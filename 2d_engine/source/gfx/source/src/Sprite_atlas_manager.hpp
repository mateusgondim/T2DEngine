#ifndef _SPRITE_ATLAS_MANAGER_HPP
#define _SPRITE_ATLAS_MANAGER_HPP

#define MAX_NUM_ATLASES 10

#include <memory>
#include <vector>
#include <string>
#include "Sprite_atlas.hpp"

//TODO: Pixels_per_unit is a global variable tha should be the same for all the sprites in the games, find a way to maintain the consistency, maybe a variable in the "Engine" class...

namespace gfx {
	class Sprite_atlas_manager {
	public:
		static Sprite_atlas_manager & instance() 
		{
			static Sprite_atlas_manager * instance = new Sprite_atlas_manager();
			return *instance;
		}
		std::shared_ptr<const Sprite_atlas> load_atlas(const std::string & file_path, const float pixels_per_unit = 16);
	private:
		Sprite_atlas_manager() : m_num_used_atlases(0), m_max_num_atlases(MAX_NUM_ATLASES) {}

		unsigned	m_num_used_atlases;
		unsigned	m_max_num_atlases;
		std::vector<std::shared_ptr<Sprite_atlas>> m_atlas_buffer;
	};
}

#endif // !_SPRITE_ATLAS_MANAGER_HPP
