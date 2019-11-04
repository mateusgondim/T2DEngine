#ifndef _VEC4_HPP

#define _VEC4_HPP
#include "vec3.hpp"
#include "mat4.hpp"

namespace math {
	struct vec4 {
		explicit vec4(float init_x = 0, float init_y = 0, float init_z = 0, float init_w = 1) : x(init_x), y(init_y), z(init_z), w(init_w) {}
		vec4(const vec3 & v, float init_w = 1) : x(v.x), y(v.y), z(v.z), w(init_w) {}

        vec4 & operator*=(const mat4 & affine);
		
		float x;  // 32 bits
		float y; // 32 bits
		float z;  // 32 bits
		float w;  // 32 bits
	};

    inline vec4 & vec4::operator*=(const mat4 & affine)
    {
            math::vec4 vec = *this;

            x = vec.x * affine.m_elements[0][0] + vec.y * affine.m_elements[1][0] + vec.z * affine.m_elements[2][0] + vec.w * affine.m_elements[3][0];
            y = vec.x * affine.m_elements[0][1] + vec.y * affine.m_elements[1][1] + vec.z * affine.m_elements[2][1] + vec.w * affine.m_elements[3][1];
            z = vec.x * affine.m_elements[0][2] + vec.y * affine.m_elements[1][2] + vec.z * affine.m_elements[2][2] + vec.w * affine.m_elements[3][2];
            w = vec.x * affine.m_elements[0][3] + vec.y * affine.m_elements[1][3] + vec.z * affine.m_elements[2][3] + vec.w * affine.m_elements[3][3];

            return *this;
    }
}
#endif // !_VEC4_HPP
