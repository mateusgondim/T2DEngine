#include "matrix_transform.hpp"

#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4.hpp"

#include <cmath>

math::mat4 math::translate(const math::vec3 & t_vec)
{
	math::mat4 T;
	T.m_elements[3][0] = t_vec.x; T.m_elements[3][1] = t_vec.y; T.m_elements[3][2] = t_vec.z;

	return T;
}

math::vec4 math::vec4_times_mat4(const math::vec4 & vec, const math::mat4 & transf)
{
	math::vec4 r;

	r.x = vec.x * transf.m_elements[0][0] + vec.y * transf.m_elements[1][0] + vec.z * transf.m_elements[2][0] + vec.w * transf.m_elements[3][0];
	r.y = vec.x * transf.m_elements[0][1] + vec.y * transf.m_elements[1][1] + vec.z * transf.m_elements[2][1] + vec.w * transf.m_elements[3][1];
	r.z = vec.x * transf.m_elements[0][2] + vec.y * transf.m_elements[1][2] + vec.z * transf.m_elements[2][2] + vec.w * transf.m_elements[3][2];
	r.w = vec.x * transf.m_elements[0][3] + vec.y * transf.m_elements[1][3] + vec.z * transf.m_elements[2][3] + vec.w * transf.m_elements[3][3];
	return r;
}


math::mat4 math::ortho(float l, float r, float b, float t, float n, float f)
{
	math::mat4 orthographic;

	orthographic.m_elements[0][0] = 2.0f / (r - l); orthographic.m_elements[0][1] = 0.0f; orthographic.m_elements[0][2] = 0.0f; orthographic.m_elements[0][3] = 0.0f;
	orthographic.m_elements[1][0] = 0.0f; orthographic.m_elements[1][1] = 2.0f / (t - b); orthographic.m_elements[1][2] = 0.0f; orthographic.m_elements[1][3] = 0.0f;
	orthographic.m_elements[2][0] = 0.0f; orthographic.m_elements[2][1] = 0.0f; orthographic.m_elements[2][2] = -2 / (f - n); orthographic.m_elements[2][3] = 0.0f;
	orthographic.m_elements[3][0] = -(r + l) / (r - l); orthographic.m_elements[3][1] = -(t + b) / (t - b); orthographic.m_elements[3][2] = -(f + n) / (f - n); orthographic.m_elements[3][3] = 1.0f;

	return orthographic;
}

math::mat4 math::rotate_y(const float theta)
{
	vec3 p(cos(theta), 0.0f, -sin(theta));
	vec3 q(0.0f, 1.0f, 0.0f);
	vec3 r(sin(theta), 0.0f, cos(theta));
	vec3 t;

	return mat4(p, q, r, t);
}