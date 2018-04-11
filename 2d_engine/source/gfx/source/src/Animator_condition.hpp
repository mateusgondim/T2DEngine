#ifndef _ANIMATOR_CONDITION_HPP
#define _ANIMATOR_CONDITION_HPP

#include "string_id.hpp"

namespace gfx {
	struct Animator_condition final {
		enum class Mode {IF, IF_NOT, GREATER, LESS, EQUALS, NOT_EQUAL};
		Mode	   m_mode;
		string_id  m_parameter_id;
		float	   m_threshold;
	};
}

#endif // !_ANIMATOR_CONDITION_HPP
