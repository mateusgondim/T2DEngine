#include "Physics_manager.hpp"

#include "World.hpp"
#include "Tile_map.hpp"
#include "vec2.hpp"


namespace physics_2d {
	Physics_manager g_physics_mgr;

	World * Physics_manager::get_world() { return m_pworld; }
	
	void Physics_manager::init(const math::vec2 & gravity)
	{
		m_pworld = new World(gravity);
	}
	
	void Physics_manager::shut_down() { delete m_pworld; }
}