#ifndef _COLLIDER_2D_DEF_HPP
#define _COLLIDER_2D_DEF_HPP

#include "AABB_2d.hpp"
namespace physics_2d {
	struct Collider_2d_def {
		AABB_2d  m_aabb;
		bool     m_is_trigger;
	};
}
#endif // !_COLLIDER_2D_DEF_HPP
