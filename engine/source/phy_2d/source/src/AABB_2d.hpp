#ifndef _AABB_2D_HPP
#define _AABB_2D_HPP
#include "vec2.hpp"
namespace physics_2d {
	struct AABB_2d final {
		AABB_2d() = default;
		AABB_2d(const math::vec2 & min, const math::vec2 & max) : p_min(min), p_max(max) {}

		math::vec2 p_min; // 8 btyes (4 byte aligned)
		math::vec2 p_max; // 8 bytes (4 byte aligned)
	};

	inline bool test_overlap(const AABB_2d & a, const AABB_2d & b) 
	{
		math::vec2 d1 = b.p_min - a.p_max;
		math::vec2 d2 = a.p_min - b.p_max;

		if (d1.x > 0.0f || d1.y > 0.0f) {
			return false;
		}

		if (d2.x > 0.0f || d2.y > 0.0f) {
			return false;
		}

		return true;
	}
}
#endif // !_AABB_2D_HPP
