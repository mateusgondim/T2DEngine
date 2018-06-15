#ifndef _VEC3_HPP

#define _VEC3_HPP	

namespace math { struct vec2; }

namespace math {
	struct vec3 {
		explicit vec3(float init_x = 0, float init_y = 0, float init_z = 0) : x(init_x), y(init_y), z(init_z) {}
		vec3(const vec2 & v);
		
		void   zero() { x = y = z = 0.0f; }
		vec3 & operator+=(const vec3 & rhs);
		vec3 & operator-=(const vec3 & v);
		
		float x; // 32 bits (4 - byte aligned)
		float y; // 32 bits (4 - byte aligned)
		float z; // 32 bits (4 - byte aligned)
	};
	
	vec3 operator+(const vec3 & lhs, const vec3 & rhs);
	vec3 operator-(const vec3 & lhs, const vec3 & rhs);
	vec3 operator-(const vec3 & v);
}

#endif // !_VEC3_HPP
