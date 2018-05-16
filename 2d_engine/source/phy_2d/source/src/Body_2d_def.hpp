#ifndef _BODY_2D_DEF_HPP
#define _BODY_2D_DEF_HPP

#include "Body_2d.hpp"
#include "AABB_2d.hpp"
#include "vec2.hpp"

namespace physics_2d {
	struct Body_2d_def {
		Body_2d::Entity_types	m_type;
		float					m_mass;
		math::vec2				m_position;
		math::vec2				m_velocity;
		math::vec2              m_vel_threshold;
		math::vec2				m_acceleration;
		AABB_2d					m_aabb;
	};
}
#endif // !_BODY_2D_DEF_HPP
