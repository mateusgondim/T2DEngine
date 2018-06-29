#include "vec3.hpp"
#include "vec2.hpp"
#include <iostream>
#include <cmath>

const math::vec3 math::g_zero_vec3(0.0f, 0.0f, 0.0f);

math::vec3::vec3(const math::vec2 & v) : x(v.x), y(v.y), z(0.0f) {}

void math::vec3::normalize() 
{
	float mag_sqare = x * x + y * y + z * z;
	if(mag_sqare > 0.0f) {
		float one_over_mag = 1.0f / sqrt(mag_sqare);
		x *= one_over_mag;
		y *= one_over_mag;
		z *= one_over_mag;
	}
}

math::vec3 & math::vec3::operator+=(const math::vec3 & rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

math::vec3 & math::vec3::operator-=(const math::vec3 & v) 
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

math::vec3 & math::vec3::operator*=(const float k) 
{
	x *= k;
	y *= k;
	z *= k;

	return *this;
}

math::vec3 & math::vec3::operator/=(const float k) 
{
	x /= k;
	y /= k;
	z /= k;

	return *this;
}

math::vec3 math::operator+(const math::vec3 & lhs, const math::vec3 & rhs)
{
	math::vec3 sum = lhs;
	sum += rhs;

	return sum;
}

math::vec3 math::operator-(const math::vec3 & lhs, const math::vec3 & rhs) 
{
	vec3 r = lhs;
	r -= rhs;
	
	return r;
}

math::vec3 math::operator-(const math::vec3 & v)
{
	math::vec3 r = v;
	r.x = -r.x;
	r.y = -r.y;
	r.z = -r.z;

	return r;
}


math::vec3 math::operator*(const float k, const math::vec3 & rhs) 
{
	math::vec3 r = rhs;
	r *= k;

	return r;
}

math::vec3 math::operator*(const math::vec3 & lhs, const float k) 
{
	math::vec3 r = lhs;
	r *= k;

	return r;
}

math::vec3 math::operator/(const float k, const math::vec3 & rhs) 
{
	math::vec3 r = rhs;
	r /= k;

	return r;
}

math::vec3 math::operator/(const math::vec3 & lhs, const float k) 
{
	math::vec3 r = lhs;
	r /= k;

	return r;
}

std::ostream & math::operator<<(std::ostream & os, const math::vec3 & v) 
{
	os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
	return os;
}