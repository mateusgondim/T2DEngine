#include "Int_condition.hpp"


bool Int_condition::check(const void * pvalue) const
{
	//expects a pointer to int
	const int * pivalue = static_cast<const int *>(pvalue);

	switch (m_op) {
	case GREATER_THAN:
		return (*pivalue > m_comp_value) ? (true) : (false);
		break;
	case LESS_THAN:
		return (*pivalue < m_comp_value) ? (true) : (false);
		break;
	case EQUALITY:
		return (*pivalue == m_comp_value) ? (true) : (false);
		break;
	case INEQUALITY:
		return (*pivalue != m_comp_value) ? (true) : (false);
	default:
		return false;
		break;
	}
}