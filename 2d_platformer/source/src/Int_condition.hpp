#ifndef _INT_CONDITION_HPP
#define _INT_CONDITION_HPP

#include "Condition.hpp"
#include <string>

/*Int_condition: This class represents condition on a integer variable, it works by
 * evaluating the logical operation specified on the m_op variable. The check(const void *) function, 
 * evaluates the operation using the m_comp_value and the value passed in the function as operands. 
 * This class should not be used directy, the Animator_controller uses it, to  add conditions on
 * transitions in the state machine
 */

class Int_condition : public Condition {
public:
	Int_condition(const std::string & var_name, const Cond_operator & op, const int comp_value) : Condition(var_name), m_op(op), m_comp_value(comp_value) {}
	
	bool check(const void * pvalue) const override;
	Type get_type() const override { return Type::INTEGER; }
	Cond_operator get_operator() const override { return m_op; }
	const void *get_comp_value() const override { return static_cast<const void *>(&m_comp_value); }
private:
	Cond_operator m_op;
	int m_comp_value;
};
	
#endif // !_INT_CONDITION_HPP
