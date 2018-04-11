#ifndef _ANIMATOR_STATE_TRANSITION_HPP
#define _ANIMATOR_STATE_TRANSITION_HPP

#include "string_id.hpp"
#include "Animator_condition.hpp"
#include <vector>
#include <ostream>

namespace gfx {
	class Animator_state_transition final {
	friend std::ostream & operator<<(std::ostream & os, const Animator_state_transition & transition);
	public:
		Animator_state_transition(const char *destination_state_name);
		Animator_state_transition(string_id destination_state_id);

		void add_condition(Animator_condition::Mode mode, float threshold, const char *parameter_name);
		void remove_condition(const Animator_condition & condition);

		const std::vector<Animator_condition> & get_conditions() const { return m_conditions; }
		string_id	get_destination_state_id() const { return m_destination_state_id; }
	private:
		string_id						 m_destination_state_id; // id for the state this transition goes to when all the conditions evaluate to true
		std::vector<Animator_condition>  m_conditions;
	};

	std::ostream & operator<<(std::ostream & os, const Animator_state_transition & transition);
}

#endif // !_ANIMATOR_STATE_TRANSITION_HPP
