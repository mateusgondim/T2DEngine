#include "vec2.hpp"


cgm::vec2 & cgm::vec2::operator+=(const vec2 & v)
{
	x += v.x;
	y += v.y;

	return *this;
}

cgm::vec2 & cgm::vec2::operator-=(const vec2 & v) 
{
	x -= v.x;
	y -= v.y;

	return *this;
}

cgm::vec2 & cgm::vec2::operator*=(const float k) 
{
	x *= k;
	y *= k;

	return *this;
}

cgm::vec2 & cgm::vec2::operator/=(const float k) 
{
	x /= k;
	y /= k;

	return *this;
}

cgm::vec2 cgm::operator-(const cgm::vec2 & v) 
{
	return cgm::vec2(-v.x, -v.y);
}

cgm::vec2 cgm::operator+(const vec2 & lhs, const vec2 & rhs)
{
	vec2 r = lhs;
	r += rhs;

	return r;
}

cgm::vec2 cgm::operator-(const vec2 & lhs, const vec2 & rhs) 
{
	vec2 r = lhs;
	r -= rhs;

	return r;
}

cgm::vec2  cgm::operator*(const float k, const vec2 & rhs) 
{
	vec2 r = rhs;
	r *= k;

	return r;
}

cgm::vec2 cgm::operator/(const float k, const vec2 & rhs) 
{
	vec2 r = rhs;
	r /= k;

	return r;
}

float cgm::operator*(const vec2 & lhs, const vec2 & rhs) 
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

bool cgm::operator==(const vec2 & lhs, const vec2 & rhs) 
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

bool cgm::operator!=(const vec2 & lhs, const vec2 & rhs) 
{
	return !(lhs == rhs);
}