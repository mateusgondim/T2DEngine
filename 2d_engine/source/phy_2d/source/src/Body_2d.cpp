#include "Body_2d.hpp"


physics_2d::Body_2d::Body_2d(const Entity_types & type, const cgm::vec2 & pos, const float m, const AABB_2d & aabb)
	: m_type(type), m_position(pos), m_mass(m), m_aabb(aabb), m_vel_threshold(cgm::vec2(0.0f, 0.0f)) {}