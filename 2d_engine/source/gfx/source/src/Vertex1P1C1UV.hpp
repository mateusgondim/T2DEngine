#ifndef _VERTEX1P1C1UV_HPP
#define _VERTEX1P1C1UV_HPP
#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"

namespace  gfx {
	struct Vertex1P1C1UV {
		Vertex1P1C1UV() = default;
		Vertex1P1C1UV(const cgm::vec3 & pos, const cgm::vec4 & col, const cgm::vec2 & uv) 
		{
			m_pos = pos;
			m_col = col;
			m_uv = uv;
		}
		cgm::vec3 m_pos;
		cgm::vec4 m_col;
		cgm::vec2 m_uv;
	};
}

#endif // !_VERTEX1P1C1UV_HPP	
