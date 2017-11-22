#ifndef _PHYSICS_MANAGER_HPP
#define _PHYSICS_MANAGER_HPP

#include "vec2.hpp"
#include "World.hpp"
#include "Tile_map.hpp"

namespace physics_2d {
	class Physics_manager final {
	public:
		Physics_manager() : m_pworld(nullptr) {}
		~Physics_manager() {}
		
		World * get_world() { return m_pworld; }
		void init(Tile_map *pmap, const cgm::vec2 & gravity = cgm::vec2(0.0f, -1.30f)) 
		{ 
			m_pworld = new World(gravity); 
			m_pworld->set_tile_map(pmap);
		}
		void shut_down() { delete m_pworld; }
	private:
		World *m_pworld;
	};
}
#endif // !_PHYSICS_MANAGER_HPP
