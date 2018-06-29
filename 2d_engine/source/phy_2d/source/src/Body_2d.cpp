#include "Body_2d.hpp"
#include "Body_2d_def.hpp"

namespace physics_2d {
	Body_2d::Body_2d(const Entity_types & type, const math::vec2 & pos, const float m, const AABB_2d & aabb)
		: m_type(type), m_position(pos), m_mass(m), m_aabb(aabb), m_vel_threshold(math::vec2(0.0f, 0.0f)) {}

	Body_2d::Body_2d(const Body_2d_def *pbody_def) :
		m_type(pbody_def->m_type), m_position(pbody_def->m_position), m_velocity(pbody_def->m_velocity), m_vel_threshold(pbody_def->m_vel_threshold),  m_mass(pbody_def->m_mass), m_aabb(pbody_def->m_aabb), m_acceleration(pbody_def->m_acceleration),
	    m_gravity_scale(pbody_def->m_gravity_scale), m_map_collision(pbody_def->m_map_collision) {}
	
}