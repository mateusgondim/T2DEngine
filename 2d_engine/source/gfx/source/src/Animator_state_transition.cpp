#include "Animator_state_transition.hpp"
#include "Animator_condition.hpp"
#include "string_id.hpp"
#include <vector>
#include <ostream>

gfx::Animator_state_transition::Animator_state_transition(const char *destination_state_name) 
{
	m_destination_state_id = intern_string(destination_state_name);
}

gfx::Animator_state_transition::Animator_state_transition(string_id destination_state_id) : m_destination_state_id(destination_state_id) {}


//TODO: MAYBE SORT THE CONDITION VECTOR, BASED ON PARAMETER'S ID
void gfx::Animator_state_transition::add_condition(gfx::Animator_condition::Mode mode, float threshold, const char *parameter_name) 
{
	// generate the parameter' name_id
	string_id name_id = intern_string(parameter_name);
	
	//create the condition
	Animator_condition condition;
	condition.m_mode = mode;
	condition.m_threshold = threshold;
	condition.m_parameter_id = name_id;

	//add to conditions
	m_conditions.push_back(condition);
}

void gfx::Animator_state_transition::remove_condition(const gfx::Animator_condition & condition) 
{
	std::vector<Animator_condition>::iterator it;
	for (it = m_conditions.begin(); it != m_conditions.end(); ++it) {
		if (it->m_parameter_id == condition.m_parameter_id) {
			m_conditions.erase(it);
			break;
		}
	}
}

std::ostream & gfx::operator<<(std::ostream & os, const gfx::Animator_state_transition & transition) 
{
	bool more_than_one_condition = false;
	os << "-> " << get_string_by_id(transition.m_destination_state_id) << std::endl;
	os << '\t' << "\t" << "if ";
	for (std::vector<Animator_condition>::const_iterator it = transition.m_conditions.cbegin(); it != transition.m_conditions.cend(); ++it) {
		if (more_than_one_condition) {
			os << " && (" << get_string_by_id(it->m_parameter_id);
		}
		else {
			os << "(" << get_string_by_id(it->m_parameter_id);
		}
		switch (it->m_mode) {
		case Animator_condition::Mode::EQUALS:
			os << "==";
			break;
		case Animator_condition::Mode::GREATER:
			os << ">";
			break;
		case Animator_condition::Mode::LESS:
			os << "<";
			break;
		case Animator_condition::Mode::NOT_EQUAL:
			os << "!=";
			break;
		case Animator_condition::Mode::IF:
			os << " trigger is true)";
			break;
		case Animator_condition::Mode::IF_NOT:
			os << " trigger is false)";
			break;
		}
		if ((it->m_mode != Animator_condition::Mode::IF) && (it->m_mode != Animator_condition::Mode::IF_NOT)) {
			os << it->m_threshold << ")";
		}
		more_than_one_condition = true;
	}
	return os;
}