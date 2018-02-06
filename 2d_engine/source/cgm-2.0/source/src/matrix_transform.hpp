#ifndef _MATRIX_TRANSFORM_HPP

#define _MATRIX_TRANSFORM_HPP

#include "mat4.hpp"
#include "vec3.hpp"
#include "vec4.hpp"

namespace cgm {
	mat4 translate(const vec3 & t_vec);
	vec4 vec4_times_mat4(const vec4 & vec, const mat4 & transf);
	mat4 ortho(float l, float r, float b, float t, float n, float f);
	mat4 rotate_y(const float theta);
}
#endif // !_MATRIX_TRANSFORM_HPP

