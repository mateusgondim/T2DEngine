#include "Physics_manager.hpp"

#include "World.hpp"
#include "Tile_map.hpp"
#include "vec2.hpp"


namespace physics_2d {
	World * Physics_manager::get_world() { return m_pworld; }
	
	void Physics_manager::init(Tile_map *pmap, const math::vec2 & gravity)
	{
		m_pworld = new World(gravity);
		m_pworld->set_tile_map(pmap);
	}
	
	void Physics_manager::shut_down() { delete m_pworld; }
}