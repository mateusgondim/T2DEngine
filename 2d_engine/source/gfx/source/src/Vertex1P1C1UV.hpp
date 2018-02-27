#ifndef _VERTEX1P1C1UV_HPP
#define _VERTEX1P1C1UV_HPP
#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"

namespace  gfx {
	struct Vertex1P1C1UV {
		Vertex1P1C1UV() = default;
		Vertex1P1C1UV(const math::vec3 & pos, const math::vec4 & col, const math::vec2 & uv)
		{
			m_pos = pos;
			m_col = col;
			m_uv = uv;
		}
		math::vec3 m_pos;
		math::vec4 m_col;
		math::vec2 m_uv;
	};
}

#endif // !_VERTEX1P1C1UV_HPP	
