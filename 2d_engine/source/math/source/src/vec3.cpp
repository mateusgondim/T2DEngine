#include "vec3.hpp"
#include "vec2.hpp"

math::vec3::vec3(const math::vec2 & v) : x(v.x), y(v.y), z(0.0f) {}

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