#ifndef _COLLISION_LISTENER_HPP
#define _COLLISION_LISTENER_HPP

#include "Body_2d.hpp"
#include "AABB_2d.hpp"

/* Collision_listener: part of the physics system, classes that want to be notified by the
 *  physics engine when a collision happens with other bodies 2d and tiles, should implement this interface
 */
namespace physics_2d {
	class Collision_listener {
	public:
		virtual void begin_body_collision(Body_2d & b1, Body_2d & b2) const = 0;
		virtual void end_body_collision(Body_2d & b1, Body_2d & b2) const = 0;

		virtual void begin_tile_collision(Body_2d & b, const AABB_2d & tile_aabb) const = 0;
		virtual void end_tile_collision(Body_2d   & b, const AABB_2d & tile_aabb) const = 0;
	};
}
#endif // !_COLLISION_LISTENER_HPP
