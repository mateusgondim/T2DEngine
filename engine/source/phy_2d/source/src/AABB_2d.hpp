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
}
#endif // !_AABB_2D_HPP
