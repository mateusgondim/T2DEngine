#include "vec3.hpp"

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