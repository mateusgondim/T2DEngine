#include "Button.hpp"

bool Button::operator<(const Button & rhs) 
{
	return this->m_bound_key < rhs.m_bound_key;
}

bool operator<(const Button & lhs, const Button & rhs) 
{
	return lhs.m_bound_key < rhs.m_bound_key;
}

void Button::update(const int action) 
{
	switch (m_state) {
	case RELEASED:
		if (action == PRESSED) {
			m_state = PRESSED;
			break;
		}
	case PRESSED:
		if (action == RELEASED) {
			m_state = RELEASED;
			break;
		}
		else if (action == REPEAT) {
			m_state == REPEAT;
			break;
		}
	case REPEAT:
		if (action == RELEASED) {
			m_state == RELEASED;
			break;
		}
		else if (action == REPEAT) {
			m_state = REPEAT;
			break;
		}
	}
}
