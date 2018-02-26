#ifndef _MATRIX_TRANSFORM_HPP

#define _MATRIX_TRANSFORM_HPP

namespace cgm { struct vec3; struct vec4; struct mat4; } //forward declaration to avoid includes

namespace cgm {
	mat4 translate(const vec3 & t_vec);
	vec4 vec4_times_mat4(const vec4 & vec, const mat4 & transf);
	mat4 ortho(float l, float r, float b, float t, float n, float f);
	mat4 rotate_y(const float theta);
}
#endif // !_MATRIX_TRANSFORM_HPP

