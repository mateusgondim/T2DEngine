#ifndef _AABB_2D_HPP
#define _AABB_2D_HPP
#include "vec2.hpp"

struct AABB_2d {
	AABB_2d() = default;
	AABB_2d(const cgm::vec2 & max, const cgm::vec2 & min) : p_max(max), p_min(min)  {}
	
	cgm::vec2 p_max;
	cgm::vec2 p_min;
};

#endif // !_AABB_2D_HPP
