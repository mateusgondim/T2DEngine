#ifndef _TRANSFORM_HPP
#define _TRANSFORM_HPP

#include "vec3.hpp"
#include "mat4.hpp"

namespace math {
	class Transform {
	public:
		Transform() = default;
		Transform(const math::vec3 & translation) : m_translation(translation) {}
		math::vec3 & get_translation() { return m_translation; }
		const math::vec3 & get_translation() const { return m_translation; }
	private:
		math::vec3	m_translation;
		math::mat4	m_obj_to_wld; // this should be a rotation matrix or, even better, a quaternion
	};
}
#endif // !_TRANSFORM_HPP
