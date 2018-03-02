#ifndef _MAT4_HPP

#define _MAT4_HPP
namespace math { struct vec3; struct vec4; }

namespace math {
	class mat4 {
	friend	mat4 translate(const vec3 & t_vec);
	friend	vec4 vec4_times_mat4(const vec4 & vec, const mat4 & transf);
	friend	mat4 ortho(float l, float r, float b, float t, float n, float f);
		
	public:
		mat4();
		mat4(const vec3 & p, const vec3 & q, const vec3 & r, const vec3 & t);
		mat4(const mat4 & L, const vec3 & t);
		mat4 & transpose();
		const float *value_ptr() const { return &m_elements[0][0]; }
	private:
		float m_elements[4][4]; // size 512 bits(64 bytes) ( 4 byte aligned)
	};
}
#endif // !_MAT4_HPP
