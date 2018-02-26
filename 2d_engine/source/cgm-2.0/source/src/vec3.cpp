#include "vec3.hpp"
#include "vec2.hpp"

cgm::vec3::vec3(const cgm::vec2 & v) : x(v.x), y(v.y), z(0.0f) {}

cgm::vec3 & cgm::vec3::operator+=(const cgm::vec3 & rhs) 
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

cgm::vec3 cgm::operator+(const cgm::vec3 & lhs, const cgm::vec3 & rhs) 
{
	cgm::vec3 sum = lhs;
	sum += rhs;

	return sum;
}

cgm::vec3 cgm::operator-(const cgm::vec3 & v) 
{
	cgm::vec3 r = v;
	r.x = -r.x;
	r.y = -r.y;
	r.z = -r.z;

	return r;
}