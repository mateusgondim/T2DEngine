#include "Animation_state.hpp"
#include <string>

/* Animation_state copy constructor works as a value-like variable,
 * so it can't point to the parameters of the Animator_controller that contains
 * anim_state, for that the pointers of the transition maps will be set to nullptr
 */

/*
Animation_state::Animation_state(const Animation_state & anim_state) : m_name(anim_state.m_name), m_animation_player(anim_state.m_animation_player), m_int_transitions(anim_state.m_int_transitions),
	m_bool_transitions(anim_state.m_bool_transitions), m_trigger_transitions(anim_state.m_trigger_transitions)
{
	clear_transitions();
}
*/
/*
void Animation_state::clear_transitions() 
{
	for (auto iter = m_int_transitions.begin(); iter != m_int_transitions.end(); ++iter) {
		(iter->second).first = nullptr;
	}

	for (auto iter = m_bool_transitions.begin(); iter != m_bool_transitions.end(); ++iter) {
		(iter->second).first = nullptr;
	}

	for (auto iter = m_trigger_transitions.begin(); iter != m_trigger_transitions.end(); ++iter) {
		iter->second = nullptr;
	}
}*/

/*
Animation_state & Animation_state::operator=(const Animation_state & rhs) 
{
	if (this != &rhs) {
		m_name				   =  rhs.m_name;
		m_animation_player     =  rhs.m_animation_player;
		m_int_transitions      =  rhs.m_int_transitions;
		m_bool_transitions     =  rhs.m_bool_transitions;
		m_trigger_transitions  =  rhs.m_trigger_transitions;
		clear_transitions();
	}
	return *this;
}*/

void Animation_state::update() 
{
	m_animation_player.update();
}
/*
std::string Animation_state::check_transition() 
{
	//loop throug each transition condition, to see if it can make a transition
	
	//integer transition parameters
	for (auto iter = m_int_transitions.begin(); iter != m_int_transitions.end(); ++iter) {
		const std::string & state_name = iter->first;
		pint_param pvarible = (iter->second).first;  // get the varible current value;
		int expected_value = (iter->second).second;

		if (pvarible -> second == expected_value) {
			return state_name;
		}
	}
	// boolean  transition parameters
	for (auto iter = m_bool_transitions.begin(); iter != m_bool_transitions.end(); ++iter) {
		const std::string & state_name = iter->first;
		pbool_param pvariable = (iter->second).first;
		bool expected_value   = (iter->second).second;

		if (pvariable ->second == expected_value) {
			return state_name;
		}
	}

	// trigger transition parameters
	for (auto iter = m_trigger_transitions.begin(); iter != m_trigger_transitions.end(); ++iter) {
		const std::string & state_name = iter->first;
		ptrigger_param pvariable = iter->second;

		if (pvariable->second == true) {
			return state_name;
		}
	}

	return std::string();
}*/

std::ostream & operator<<(std::ostream & os, const Animation_state & anim_state) 
{
	os << "State name: "  << anim_state.m_name << std::endl;
	
	/*
	for (auto iter = anim_state.m_int_transitions.cbegin(); iter != anim_state.m_int_transitions.cend(); ++iter) {
		os << "\t -> ";
		os << iter->first << " if (" << ((iter->second).first)->first << " == " << (iter->second).second << ")" << std::endl;
	}

	for (auto iter = anim_state.m_bool_transitions.cbegin(); iter != anim_state.m_bool_transitions.cend(); ++iter) {
		os << "\t -> ";
		os << iter->first << " if (" << ((iter->second).first)->first << " == " << (iter->second).second << ")" << std::endl;
	}

	for (auto iter = anim_state.m_trigger_transitions.cbegin(); iter != anim_state.m_trigger_transitions.cend(); ++iter) {
		os << "\t -> ";
		os << iter->first << " if (" << iter->second->first << " == true)" << std::endl;
	}*/

	return os;
}

bool operator==(const Animation_state & lhs, const  Animation_state & rhs) 
{
	return lhs.get_name() == rhs.get_name();
}

bool operator!=(const Animation_state & lhs, const Animation_state & rhs) 
{
	return !(lhs == rhs);
}