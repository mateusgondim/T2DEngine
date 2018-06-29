#ifndef _VEC3_HPP

#define _VEC3_HPP	
#include <iostream>
#include <cmath>

namespace math { struct vec2; }

namespace math {
	struct vec3 {
		explicit vec3(float init_x = 0, float init_y = 0, float init_z = 0) : x(init_x), y(init_y), z(init_z) {}
		vec3(const vec2 & v);
		
		void   zero() { x = y = z = 0.0f; }
		vec3 & operator+=(const vec3 & rhs);
		vec3 & operator-=(const vec3 & v);
		
		vec3 & operator*=(const float k);
		vec3 & operator/=(const float k);

		void normalize();

		float x; // 32 bits (4 - byte aligned)
		float y; // 32 bits (4 - byte aligned)
		float z; // 32 bits (4 - byte aligned)
	};
	
	vec3 operator+(const vec3 & lhs, const vec3 & rhs);
	vec3 operator-(const vec3 & lhs, const vec3 & rhs);
	vec3 operator-(const vec3 & v);
	
	vec3 operator*(const vec3 & lhs, const float k);
	vec3 operator*(const float k, const vec3 & rhs);
	vec3 operator/(const vec3 & lhs, const float k);
	vec3 operator/(const float k, const vec3 & rhs);

	std::ostream & operator<<(std::ostream & os, const vec3 & v);

	//dot product
	inline float operator*(const vec3 & lhs, const vec3 & rhs)
	{
		return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
	}

	inline bool operator==(const vec3 & lhs, const vec3 & rhs)
	{
		return (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z);
	}

	inline bool operator!=(const vec3 & lhs, const vec3 & rhs)
	{
		return !(lhs == rhs);
	}

	inline float vec3_mag(const vec3 & v) 
	{
		return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	}

	extern const vec3 g_zero_vec3;
}

#endif // !_VEC3_HPP
