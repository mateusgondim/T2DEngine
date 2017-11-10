#ifndef _VEC2_HPP

#define _VEC2_HPP
namespace cgm {
	struct vec2 {
		explicit vec2(float init_x = 0, float init_y = 0) : x(init_x), y(init_y) {}

		//zero the vector
		void zero()  { x = y = 0.0f; }

		//compound vector addition and subtraction
		vec2 & operator+=(const vec2 & v);
		vec2 & operator-=(const vec2 & v);

		//vector compound multiplication and division by scalar
		vec2 & operator*=(const float k);
		vec2 & operator/=(const float k);

		float x;
		float y;
	};
	
	//unary minus operator for negating the vector
	vec2  operator-(const cgm::vec2 & v);

	vec2   operator+(const vec2 & lhs, const vec2 & rhs);
	vec2   operator-(const vec2 & lhs, const vec2 & rhs);
	vec2   operator*(const float k,    const vec2 & rhs);
	vec2   operator/(const float k,    const vec2 & rhs);

	//dot product
	float operator*(const vec2 & lhs, const vec2 & rhs);

	//equality and inequality operators
	bool operator==(const vec2 & lhs, const vec2 & rhs);
	bool operator!=(const vec2 & lhs, const vec2 & rhs);
}


#endif // !_VEC2_HPP

