#include "Animator_controller.hpp"

#include "Animator_controller_parameter.hpp"
#include "Animator_state.hpp"
#include "Animation_player.hpp"
#include "Animator_state_transition.hpp"
#include "Animator_condition.hpp"
#include "string_id.hpp"
#include <iostream>
#include <cassert>


gfx::Animator_state & gfx::Animator_controller::add_state(const char *name) 
{
	// generate an id from the state's name
	string_id state_name_id = intern_string(name);

	//check if there is no state with this id in the state machine
	std::map<string_id, gfx::Animator_state>::iterator it = m_state_machine.find(state_name_id);
	if (it == m_state_machine.end() ) { // there is not state with this id
		Animator_state state(state_name_id);
		m_state_machine[state_name_id] = state;
		
		//if this is the first added state, set it to be the current state
		if (m_state_machine.size() == 1) {
			m_current_state_id = state_name_id;
		}
	}
	return m_state_machine[state_name_id];
}

gfx::Animator_state & gfx::Animator_controller::add_state(const char *name, const gfx::Animation_player & anim_player) 
{
	// generate an id from the state's name
	string_id state_name_id = intern_string(name);

	//check if there is no state with this id in the state machine
	std::map<string_id, gfx::Animator_state>::iterator it = m_state_machine.find(state_name_id);
	if (it == m_state_machine.end()) { // there is not state with this id
		Animator_state state(state_name_id, anim_player);
		m_state_machine[state_name_id] = state;

		//if this is the first added state, set it to be the current state
		if (m_state_machine.size() == 1) {
			m_current_state_id = state_name_id;
		}
	}
	return m_state_machine[state_name_id];
}

void gfx::Animator_controller::remove_state(const gfx::Animator_state & state) 
{
	//check if there is a state with this id
	std::map<string_id, Animator_state>::iterator it = m_state_machine.find(state.get_state_id());
	if (it != m_state_machine.end()) {
		m_state_machine.erase(it);
	}
}

void gfx::Animator_controller::set_current_state(const char *name) 
{
	// generate the id for this state name
	string_id state_name_id = intern_string(name);

	//check wheter this state is on the machine
	std::map<string_id, Animator_state>::iterator it = m_state_machine.find(state_name_id);
	if (it != m_state_machine.end()) {
		//this state exists
		m_current_state_id = state_name_id;
	}
}

void gfx::Animator_controller::add_parameter(const char *name, const Animator_controller_parameter::Type type) 
{
	// generate a unique id from the parameter's name
	string_id name_id = intern_string(name);

	//check if there is already a parameter with this id on the map
	std::map<string_id, Animator_controller_parameter>::iterator it = m_parameters.find(name_id);

	if (it == m_parameters.end()) { // there is no parameter with this id, so add it to the map
		m_parameters[name_id] = Animator_controller_parameter(name_id, type);
	}
}

void gfx::Animator_controller::add_parameter(const Animator_controller_parameter & parameter) 
{
	string_id  name_id = parameter.get_id();

	//check if there is already a parameter with this id on the map
	std::map<string_id, Animator_controller_parameter>::iterator it = m_parameters.find(name_id);

	if (it == m_parameters.end()) { // there is no parameter with this id, so add it to the map
		m_parameters[name_id] = parameter;
	}
}

void gfx::Animator_controller::set_integer(const string_id id, const int ival) 
{
	// find the parameter with name_id = id
	std::map<string_id, Animator_controller_parameter>::iterator it = m_parameters.find(id);

	if ( it != m_parameters.end() ) { // if there is a parameter with this id on the map
		// check if this parameter is really an integer parameter
		if ( (it->second).get_type() == Animator_controller_parameter::Type::INT) {
			//change the parameter's value
			(it->second).set_value(ival);
			m_param_val_changed = true;
		}
	}
}

void gfx::Animator_controller::set_integer(const char *name, const int ival) 
{
	//get the id for this name
	string_id id = intern_string(name);
	//set the parameter
	set_integer(id, ival);
}

void gfx::Animator_controller::set_bool(const string_id id, const bool bval) 
{
	// find the parameter with name_id = id
	std::map<string_id, Animator_controller_parameter>::iterator it = m_parameters.find(id);

	if (it != m_parameters.end()) { // if there is a parameter with this id on the map
									// check if this parameter is really an boolean parameter
		if ((it->second).get_type() == Animator_controller_parameter::Type::BOOL) {
			//change the parameter's value
			(it->second).set_value(bval);
			m_param_val_changed = true;
		}
	}
}

void gfx::Animator_controller::set_bool(const char *name, const bool bval) 
{
	//get the id for this name
	string_id id = intern_string(name);
	//set the parameter
	set_bool(id, bval);
}

void gfx::Animator_controller::set_trigger(const string_id id) 
{
	// find the parameter with name_id = id
	std::map<string_id, Animator_controller_parameter>::iterator it = m_parameters.find(id);

	if (it != m_parameters.end()) { // if there is a parameter with this id on the map
	   // check if this parameter is really an trigger parameter
		if ((it->second).get_type() == Animator_controller_parameter::Type::TRIGGER) {
			//change the parameter's value
			(it->second).set_value(true);
			m_param_val_changed = true;
		}
	}
}

void gfx::Animator_controller::set_trigger(const char *name) 
{
	//get the id for this name
	string_id id = intern_string(name);
	//set the parameter
	set_trigger(id);
}

int gfx::Animator_controller::get_integer(const string_id id) const 
{
	//search for the parameter in the map
	std::map<string_id, Animator_controller_parameter>::const_iterator it = m_parameters.find(id);

	if (it != m_parameters.end()) { //if the parameter exists  
		if ((it->second).get_type() == Animator_controller_parameter::Type::INT) { // check if the parameter's type is really an int
			return (it->second).get_int();
		}
		return 0;
	}
	// in case of errors return 0
	return 0;
}

int gfx::Animator_controller::get_integer(const char *name) const 
{
	string_id id = intern_string(name);
	return get_integer(id);
}

bool gfx::Animator_controller::get_bool(const string_id id) const 
{
	//search for the parameter in the map
	std::map<string_id, Animator_controller_parameter>::const_iterator it = m_parameters.find(id);

	if (it != m_parameters.end()) { //if the parameter exists  
		if ((it->second).get_type() == Animator_controller_parameter::Type::BOOL) { // check if the parameter's type is really an bool
			return (it->second).get_bool();
		}
		return false;
	}
	// in case of errors return false
	return false;
}


bool gfx::Animator_controller::get_bool(const char *name) const 
{
	string_id id = intern_string(name);
	return get_bool(id);
}

bool gfx::Animator_controller::get_trigger(const string_id id) const 
{
	//search for the parameter in the map
	std::map<string_id, Animator_controller_parameter>::const_iterator it = m_parameters.find(id);

	if (it != m_parameters.end()) { //if the parameter exists  
		if ((it->second).get_type() == Animator_controller_parameter::Type::TRIGGER) { // check if the parameter's type is really an bool
			return (it->second).get_bool();
		}
		return false;
	}
	// in case of errors return false
	return false;
}

bool gfx::Animator_controller::get_trigger(const char *name) const
{
	string_id id = intern_string(name);
	return get_trigger(id);
}

bool gfx::Animator_controller::evaluate_condition(const gfx::Animator_controller_parameter & parameter, const gfx::Animator_condition & condition) const
{
	//Switch on the parameter's type
	switch (parameter.get_type()) {
	case Animator_controller_parameter::Type::FLOAT:
		//switch on the condition's operation for a FLOAT parameter
		switch (condition.m_mode) {
		case Animator_condition::Mode::EQUALS:
			return (parameter.get_float() == condition.m_threshold) ? (true) : (false);
		
		case Animator_condition::Mode::GREATER:
			return (parameter.get_float() > condition.m_threshold) ? (true) : (false);

		case Animator_condition::Mode::LESS:
			return (parameter.get_float() < condition.m_threshold) ? (true) : (false);

		case Animator_condition::Mode::NOT_EQUAL:
			return (parameter.get_float() != condition.m_threshold) ? (true) : (false);
		}
		break;
	case Animator_controller_parameter::Type::INT:
		//switch on the condition's operation for a INT parameter
		switch (condition.m_mode) {
		case Animator_condition::Mode::EQUALS:
			return (parameter.get_int() == static_cast<int>(condition.m_threshold)) ? (true) : (false);

		case Animator_condition::Mode::GREATER:
			return (parameter.get_int() > static_cast<int>(condition.m_threshold)) ? (true) : (false);

		case Animator_condition::Mode::LESS:
			return (parameter.get_int() < static_cast<int>(condition.m_threshold)) ? (true) : (false);

		case Animator_condition::Mode::NOT_EQUAL:
			return (parameter.get_int() != static_cast<int>(condition.m_threshold)) ? (true) : (false);
		}
		break;
	case Animator_controller_parameter::Type::BOOL:
		//switch on the condition's operation for a BOOL parameter
		switch (condition.m_mode) {
		case Animator_condition::Mode::EQUALS:
			return (parameter.get_bool() == static_cast<bool>(condition.m_threshold)) ? (true) : (false);

		case Animator_condition::Mode::NOT_EQUAL:
			return (parameter.get_bool() != static_cast<bool>(condition.m_threshold)) ? (true) : (false);
		}
		break;
	case Animator_controller_parameter::Type::TRIGGER:
		//switch on the condition's operation for a Trigger parameter
		switch (condition.m_mode) {
		case Animator_condition::Mode::IF:
			return (parameter.get_bool() == true) ? (true) : (false);
		
		case Animator_condition::Mode::IF_NOT:
			return (parameter.get_bool() == false) ? (true) : (false);
		}
		break;
	}
}


void gfx::Animator_controller::update(const float dt)
{
	//get the current state
	std::map<string_id, Animator_state>::iterator it = m_state_machine.find(m_current_state_id);
	Animator_state & curr_state = it->second;

	if (m_param_val_changed) {
		//get all the transitions that leaves the current state
		std::vector<Animator_state_transition> & transitions = curr_state.get_transitions();

		//evaluate each transition, taking the first which has all the conditions hold
		for (std::vector<Animator_state_transition>::iterator transition_it = transitions.begin(); transition_it != transitions.end(); ++transition_it) { //for each transition
			//evaluate all of the conditions of the current transition
			const std::vector<Animator_condition> & conditions = transition_it->get_conditions();

			// use this vector to keep track of triggers that need to be set to false if, a transition contaning triggers is taken
			std::vector<string_id> triggers_to_reset;

			bool make_transition = true;
			for (std::vector<Animator_condition>::const_iterator condition_it = conditions.begin(); condition_it != conditions.end(); ++condition_it) { // for each condition
				//get the parameter of the condition
				std::map<string_id, Animator_controller_parameter>::iterator param_it = m_parameters.find(condition_it->m_parameter_id);
				//evaluate the condition
				if (param_it != m_parameters.end()) {
					make_transition = evaluate_condition(param_it->second, *condition_it);

					//if the condition evaluated to true and involves a trigger variable
					if (make_transition && ((param_it->second).get_type() == Animator_controller_parameter::Type::TRIGGER) ) { 
						/* The parameter is a trigger value, if its value is 'true', then, we must set it to false if all the
						 * conditions of this transition evaluates to true. If it is 'false', we can only accept this condition as true if,
						 * the animator_state already played the animation.
						 */
						bool trigger_value = (param_it->second).get_bool();
						if (trigger_value) { // the trigger is true, add to the vector of triggers to reset, if all conditions evaluate to true
							triggers_to_reset.push_back((param_it->second).get_id());
						}
						else if (curr_state.is_anim_playing()) { // a false trigger, its condition can only be 'true' if the animation already played
							make_transition = false;
						}
					}
				}
				else {
					std::cerr << "ERROR(" << __FUNCTION__ << "): INVALID PARAMETER ID '" << condition_it->m_parameter_id << "' on condition variable" << std::endl;
				}
				if (!make_transition) { // if some condition did not evaluate to true
					break;
				}
			}
			if (make_transition) { // if all of the conditions held, make the transition
				m_param_val_changed = false;
				//check whether there is pending triggers to reset
				for (std::vector<string_id>::iterator it = triggers_to_reset.begin(); it != triggers_to_reset.end(); ++it) {
					std::map<string_id, Animator_controller_parameter>::iterator param_it = m_parameters.find(*it);
					(param_it->second).set_value(false);
					m_param_val_changed = true;
				}

				// stop the current state animations
				curr_state.pause_anim();

				//change the current state to the transition's destination state
				m_current_state_id = transition_it->get_destination_state_id();
				it = m_state_machine.find(m_current_state_id);
				if (it != m_state_machine.end()) {
					//update animation
					//(it->second).update(dt);
					(it->second).start_anim();
				}
				else {
					std::cerr << "ERROR(" << __FUNCTION__ << "): Invalid Animator_state id ' " << m_current_state_id << "' " << std::endl;
				}
				return;
			}
		}
	}
	//if none of the conditios held, update the current state
	curr_state.update(dt);
}

gfx::Animator_state & gfx::Animator_controller::get_current_state()
{
	std::map<string_id, Animator_state>::iterator it = m_state_machine.find(m_current_state_id);
	assert(it != m_state_machine.end());

	return it->second;	
}



std::ostream & gfx::operator<<(std::ostream & os, const Animator_controller & anim_controller)
{
	os << "---------------Animator Controller--------------------" << std::endl;
	os << "Parameters:" << std::endl;
	for (std::map<string_id, Animator_controller_parameter>::const_iterator it = anim_controller.m_parameters.cbegin(); it != anim_controller.m_parameters.cend(); ++it) {
		switch ((it->second).get_type()) {
		case Animator_controller_parameter::Type::FLOAT:
			os << "float " << get_string_by_id( (it->second).get_id()) << " = " << (it->second).get_float() << std::endl;
			break;
		case Animator_controller_parameter::Type::INT:
			os << "int " << get_string_by_id((it->second).get_id()) << " = " << (it->second).get_int() << std::endl;
			break;
		case Animator_controller_parameter::Type::BOOL:
			os << "bool " << get_string_by_id((it->second).get_id()) << " = " << (it->second).get_bool() << std::endl;
			break;
		case Animator_controller_parameter::Type::TRIGGER:
			os << "trigger " << get_string_by_id((it->second).get_id()) << " = " << (it->second).get_bool() << std::endl;
			break;
		}
	}
	
	os << "States:" << std::endl;
	for (auto iter = anim_controller.m_state_machine.cbegin(); iter != anim_controller.m_state_machine.cend(); ++iter) {
		os << (iter->second) << std::endl;
	}
	os << "--------------------------------------------------------" << std::endl;
	return os;
}
