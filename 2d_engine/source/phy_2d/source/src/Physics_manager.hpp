#ifndef _PHYSICS_MANAGER_HPP
#define _PHYSICS_MANAGER_HPP

#include "vec2.hpp"

namespace physics_2d { class World; }
class Tile_map;

namespace physics_2d {
	class Physics_manager final {
	public:
		Physics_manager() : m_pworld(nullptr) {}
		~Physics_manager() {}
		
		World * get_world();
		
		void init(Tile_map *pmap, const math::vec2 & gravity = math::vec2(0.0f, -0.9f));
		void shut_down();
	private:
		World *m_pworld;
	};
}
#endif // !_PHYSICS_MANAGER_HPP
