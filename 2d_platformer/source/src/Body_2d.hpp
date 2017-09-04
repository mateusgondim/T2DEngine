#ifndef _BODY_2D_HPP
#define _BODY_2D_HPP
#include "vec2.hpp"
#include "AABB_2d.hpp"

/* Class that defines a physical entity, all the objects that want to be affected by
 * the physics  system should inherent from this class
 */

namespace tps {
	class Body_2d {
	public:
		enum Entity_types {KINEMATIC, DYNAMIC};
		enum Solver_methods {DISPLACE, ELASTIC};
	private:
		cgm::vec2	m_position;
		cgm::vec2	m_velocity;
		cgm::vec2	m_acceleration;
		AABB_2d     m_aabb;
	};
}
#endif