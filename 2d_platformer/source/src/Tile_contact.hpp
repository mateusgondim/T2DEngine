#ifndef _TILE_CONTACT_HPP
#define _TILE_CONTACT_HPP

#include "AABB_2d.hpp"
#include "Body_2d.hpp"

/* Data structure used by the physics system to
 * store information about collision of bodies and tiles 
 */

namespace physics_2d {
	class Tile_contact {
	friend class World;
	
	private:
		Tile_contact() = default;
		AABB_2d m_tile_aabb;
		Body_2d *m_pbody;
	};
}

#endif // !_TILE_CONTACT_HPP
