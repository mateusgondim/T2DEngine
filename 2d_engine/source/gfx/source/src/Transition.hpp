#ifndef _TRANSITION_HPP
#define _TRANSITION_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Condition.hpp"


/* Transition: This class represents a transition from two states on a
 * state machine, each transition have a set of conditions. 
 * For the state machine be able to transition beetheen two states, 
 * all the conditions on that transition must evaluate to "true".
 * This class should not be used directy, the  Animator_controller will create then
 * when the operation to add transition is used.
 */
namespace gfx {
	class Transition {
		friend std::ostream & operator<<(std::ostream & os, const Transition & t);
	public:
		Transition(const std::string transition_name, const std::string & o_state_name, const std::string & d_state_name) :
			m_name(transition_name), m_o_state_name(o_state_name), m_d_state_name(d_state_name) {}


		void  add_integer_condition(const std::string & param_name, const Condition::Cond_operator & op, const int icomp_value);
		void  add_boolean_condition(const std::string & param_name, const bool bcomp_value);
		void  add_trigger_condition(const std::string & param_name);

		const std::vector<std::shared_ptr<Condition>> & get_conditions() const { return m_upconditions; }
		std::string	get_name()  const { return m_name; }
		std::string get_beginning_state_name() const { return m_o_state_name; }
		std::string get_ending_state_name()    const { return m_d_state_name; }
	private:
		std::string m_name;
		std::string m_o_state_name;
		std::string m_d_state_name;
		std::vector<std::shared_ptr<Condition>> m_upconditions;
	};

	std::ostream & operator<<(std::ostream & os, const Transition & t);
}

#endif // !_TRANSITION_HPP
