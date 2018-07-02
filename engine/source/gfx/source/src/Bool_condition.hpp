#ifndef _BOOL_CONDITION_HPP
#define _BOOL_CONDITION_HPP
#include "Condition.hpp"
#include <string>

/*Bool_condition: This class represents a boolean condition, it workds by
 * evaluating if the m_comp_value is equal to the value passed to check(const void *).
 * This class sould not be used directy, the Animator_controller uses it, to  add conditions on
 * transitions in the state machine
 */

namespace gfx {
	class Bool_condition : public Condition {
	public:
		Bool_condition(const std::string & var_name, const bool comp_value) : Condition(var_name), m_comp_value(comp_value) {}
		bool check(const void * pvalue) const override
		{
			const bool *pbvalue = static_cast<const bool *>(pvalue);
			return (*pbvalue == m_comp_value) ? (true) : (false);
		}
		Type get_type() const override { return Type::BOOLEAN; }
		Cond_operator get_operator() const override { return Cond_operator::EQUALITY; }
		const void *get_comp_value() const override { return &m_comp_value; }

	private:
		bool m_comp_value;
	};
}
#endif // !_BOOL_CONDITION_HPP
