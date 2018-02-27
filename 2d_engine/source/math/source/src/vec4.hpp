#ifndef _VEC4_HPP

#define _VEC4_HPP
#include "vec3.hpp"

namespace math {
	struct vec4 {
		explicit vec4(float init_x = 0, float init_y = 0, float init_z = 0, float init_w = 1) : x(init_x), y(init_y), z(init_z), w(init_w) {}
		vec4(const vec3 & v, float init_w = 1) : x(v.x), y(v.y), z(v.z), w(init_w) {}
		
		float x; 
		float y;
		float z;
		float w;
	};
}
#endif // !_VEC4_HPP
