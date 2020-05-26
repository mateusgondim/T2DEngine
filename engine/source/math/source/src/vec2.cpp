#include "vec2.hpp"


const math::vec2 math::g_zero_vec2(0.0f, 0.0f);

math::vec2::vec2(const math::vec2 & v) : x(v.x), y(v.y) {}


math::vec2 & math::vec2::operator+=(const vec2 & v)
{
	x += v.x;
	y += v.y;

	return *this;
}

math::vec2 & math::vec2::operator-=(const vec2 & v)
{
	x -= v.x;
	y -= v.y;

	return *this;
}

math::vec2 & math::vec2::operator*=(const float k)
{
	x *= k;
	y *= k;

	return *this;
}

math::vec2 & math::vec2::operator/=(const float k)
{
	x /= k;
	y /= k;

	return *this;
}

math::vec2 math::operator-(const math::vec2 & v)
{
	return math::vec2(-v.x, -v.y);
}

math::vec2 math::operator+(const vec2 & lhs, const vec2 & rhs)
{
	vec2 r = lhs;
	r += rhs;

	return r;
}

math::vec2 math::operator-(const vec2 & lhs, const vec2 & rhs)
{
	vec2 r = lhs;
	r -= rhs;

	return r;
}

math::vec2  math::operator*(const float k, const vec2 & rhs)
{
	vec2 r = rhs;
	r *= k;

	return r;
}

math::vec2 math::operator/(const float k, const vec2 & rhs)
{
	vec2 r = rhs;
	r /= k;

	return r;
}

math::vec2 math::operator/(const vec2 & lhs, const float k)
{
        vec2 r = lhs;
        r /= k;

        return r;
}


float math::operator*(const vec2 & lhs, const vec2 & rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

bool math::operator==(const vec2 & lhs, const vec2 & rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

bool math::operator!=(const vec2 & lhs, const vec2 & rhs)
{
	return !(lhs == rhs);
}

std::ostream & math::operator<<(std::ostream & os, const vec2 & rhs) 
{
	os << "[" << rhs.x << ", " << rhs.y << "]";
	return os;
}