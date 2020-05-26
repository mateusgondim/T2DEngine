#ifndef _VEC2_HPP

#define _VEC2_HPP

#include <cmath>
#include <iostream>

namespace math {
	struct vec2 {
		explicit vec2(float init_x = 0, float init_y = 0) : x(init_x), y(init_y) {}
		vec2(const vec2 & v);

		//zero the vector
		void zero()  { x = y = 0.0f; }

		//compound vector addition and subtraction
		vec2 & operator+=(const vec2 & v);
		vec2 & operator-=(const vec2 & v);

		//vector compound multiplication and division by scalar
		vec2 & operator*=(const float k);
		vec2 & operator/=(const float k);

		float x; // 32 bits(4 - byte alignment)
		float y; // 32 bits(4 - byte alignment)
	};
	
	//unary minus operator for negating the vector
	vec2  operator-(const math::vec2 & v);

	vec2   operator+(const vec2 & lhs, const vec2 & rhs);
	vec2   operator-(const vec2 & lhs, const vec2 & rhs);
	vec2   operator*(const float k,    const vec2 & rhs);
	vec2   operator/(const float k,    const vec2 & rhs);
    vec2   operator/(const vec2 & lhs, const float k);


	//dot product
	float operator*(const vec2 & lhs, const vec2 & rhs);

	//equality and inequality operators
	bool operator==(const vec2 & lhs, const vec2 & rhs);
	bool operator!=(const vec2 & lhs, const vec2 & rhs);

	//output
	std::ostream & operator<<(std::ostream & os, const vec2 & rhs);

	//magnitude
	inline float vec2_mag(const vec2 & v) 
	{
		return sqrt(v.x * v.x + v.y * v.y);
	}

	extern const vec2 g_zero_vec2;
}


#endif // !_VEC2_HPP

