#ifndef _CONDITION_HPP
#define _CONDITION_HPP

#include <string>

/*Condition: Abstract Base Class used to create a logical condition. This class
 *  should not be used directly, it is used by the Animator_controller
 * to add logical conditions on the transitions in the Animation state machine
 */
namespace gfx {
	class Condition {
	public:
		enum Type { INTEGER, BOOLEAN };
		enum Cond_operator { GREATER_THAN, LESS_THAN, EQUALITY, INEQUALITY };

		Condition(const std::string & var_name) : m_var_name(var_name) {}

		virtual bool check(const void * pvalue) const = 0;    // evaluates the condition using the value pointed by pvalue
		virtual const void *get_comp_value()    const = 0;   //returns a pointer to the value stored for the condition
		virtual Type get_type() const = 0;                  //returns the type of the varaible used in this condition
		virtual Cond_operator get_operator() const = 0;   // returns the type of operator this condition uses

		std::string  get_variable_name() const { return m_var_name; }

		virtual ~Condition() {};

	private:
		std::string m_var_name;
	};
}
#endif // !_CONDITION_HPP
