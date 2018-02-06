#ifndef _VEC3_HPP

#define _VEC3_HPP	
#include "vec2.hpp"
namespace cgm {
	struct vec3 {
		explicit vec3(float init_x = 0, float init_y = 0, float init_z = 0) : x(init_x), y(init_y), z(init_z) {}
		vec3(const vec2 & v) : x(v.x), y(v.y), z(0.0f) {}
		
		void   zero() { x = y = z = 0.0f; }
		vec3 & operator+=(const vec3 & rhs);
		
		float x;
		float y;
		float z;
	};
	
	vec3 operator+(const vec3 & lhs, const vec3 & rhs);
	vec3 operator-(const vec3 & v);
}

#endif // !_VEC3_HPP
