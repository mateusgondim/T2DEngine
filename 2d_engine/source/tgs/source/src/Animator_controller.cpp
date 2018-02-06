#include "Animator_controller.hpp"
#include <iostream>


void Animator_controller::add_state(const Animation_state & anim_state) 
{ 
	if (m_state_machine.empty()) {
		m_initial_state = anim_state.get_name();
		m_current_state = m_initial_state;
	}

	m_state_machine.insert({ anim_state.get_name() , anim_state}); 
}

void Animator_controller::update(const float dt) 
{
	//check triggers
	auto iter = m_state_machine.find(m_current_state);
	if (!iter->second.get_anim_player().is_playing() && m_pending_trigger ) {
		//std::cout << "LOOKING FOR TRIGGERS" << std::endl;
		for (auto b = m_trigger_parameters.begin(); b != m_trigger_parameters.end(); ++b) {
			if (b->second) {
				b->second = false;
				m_pending_trigger = false;
				m_param_val_changed = true;
			//	std::cout << "FOUND TRUE TRIGGER" << std::endl;
				break;
			}
		}
		
	}
	std::cout << "CONTROLLER UPDATE" << std::endl;
	//check if a parameter was changed
	if (m_param_val_changed) {
		//for every transition that goes from the current state
		for (auto iter_transition = m_transitions.begin(); iter_transition != m_transitions.end(); ++iter_transition) {
			if (m_current_state == (iter_transition->second).get_beginning_state_name()) {
				//check if the conditions hold
				auto iter_condition = (iter_transition->second).get_conditions().begin();
				bool expression_value = false;

				for (; iter_condition != (iter_transition->second).get_conditions().end(); ++iter_condition) {
					//get the parameter name and type
					std::string param_name = (*iter_condition)->get_variable_name();
					Condition::Type type =   (*iter_condition)->get_type();
					decltype (m_bool_parameters.begin()) iter_bool;
					decltype(m_int_parameters.begin()) iter_int;
					switch (type) {
					case Condition::Type::BOOLEAN:
						//get the parameter current value
						 iter_bool = m_bool_parameters.find(param_name);
						if (iter_bool != m_bool_parameters.end()) { //parameter found, is a boolean
							expression_value = (*iter_condition)->check(&(iter_bool->second));
						}
						else {
							auto iter_trigger = m_trigger_parameters.find(param_name);
							if (iter_trigger != m_trigger_parameters.end()) { //parameter found, is a trigger
							 expression_value = (*iter_condition)->check(&(iter_trigger->second));
							}
							else {
								//error, parameter not found
								std::cerr << __FUNCTION__ << ": Error, boolean parameter " << param_name << " not found" << std::endl;
								expression_value = false;
							}
						}
						break;
					case Condition::Type::INTEGER:
						//get the parameter's current value 
						iter_int = m_int_parameters.find(param_name);
						if (iter_int != m_int_parameters.end()) {
							expression_value = (*iter_condition)->check(&(iter_int->second));
						}
						else {
							std::cerr << __FUNCTION__ << ": Error, int parameter " << param_name << " not found" << std::endl;
							expression_value = false;
						}
						break;
					default:
						expression_value = false;
						std::cerr << __FUNCTION__ << ": Error, invalid Type" <<  std::endl;
						break;
					}
					if (!expression_value) { //if a condition failed, we cant make this transition, break from this loop
						break;
					}
				}
				//if the loop evaluated all the conditions to true
				if ( (iter_condition == (iter_transition->second).get_conditions().end()) && expression_value ) { // all the conditions evaluated to true
					//make the transition
					auto iter_state = m_state_machine.find(m_current_state);
					(iter_state->second).stop_animation();
					
					//m_state_machine[m_current_state].stop_animation();
					m_current_state = (iter_transition->second).get_ending_state_name();
					
					
					iter_state = m_state_machine.find(m_current_state);
					(iter_state->second).start_animation();
					//m_state_machine[m_current_state].start_animation();
					break;
				}
			}
		}
		m_param_val_changed = false;
	}
	auto iter_state = m_state_machine.find(m_current_state);
	(iter_state->second).update(dt);
	//m_state_machine[m_current_state].update();
}

void Animator_controller::set_initial_state(const std::string & name) 
{
	if ( (m_state_machine.find(name) != m_state_machine.end()) && (m_current_state.empty()) ) {
		m_initial_state = name;
		if (m_current_state.empty()) {
			m_current_state = name;
		}
	}
}

void Animator_controller::set_int(const std::string & name, const int ivalue) 
{
	auto iter = m_int_parameters.find(name);
	if (iter == m_int_parameters.end()) {
		return;
	}

	iter->second = ivalue;

	m_param_val_changed = true;
}

void Animator_controller::set_bool(const std::string & name, const bool bvalue) 
{
	auto iter = m_bool_parameters.find(name);
	if (iter == m_bool_parameters.end()) {
		return;
	}

	iter->second = bvalue;

	m_param_val_changed = true;
}

void Animator_controller::set_trigger(const std::string & name) 
{
	auto iter = m_trigger_parameters.find(name);
	if (iter == m_trigger_parameters.end()) {
		std::cout << "DID NOT FIND VALUE " << std::endl;
		return;
	}

	iter->second = true;

	m_param_val_changed = true;
	m_pending_trigger = true;
}

bool Animator_controller::get_trigger(const std::string & name) const 
{
	auto iter = m_trigger_parameters.find(name);
	if (iter == m_trigger_parameters.end()) {
		std::cerr << __FUNCTION__  <<": DID NOT FIND TRIGGER PARAMENTER '" << name << "'" << std::endl;
		return false;
	}
	return iter->second;
}


void Animator_controller::add_int_condition(const std::string & t_name, const std::string & param_name, Condition::Cond_operator op, const int expected_value)
{
	//find the transition to add the condition
	auto iter = m_transitions.find(t_name);
	if (iter != m_transitions.end()) {
		//add the condition
		(iter->second).add_integer_condition(param_name, op, expected_value);
	}
}

void Animator_controller::add_bool_condition(const std::string & t_name, const std::string & param_name, const bool expected_value) 
{
	auto iter = m_transitions.find(t_name);
	if (iter != m_transitions.end()) {
		(iter->second).add_boolean_condition(param_name, expected_value);
	}
}

void Animator_controller::add_trigger_condition(const std::string & t_name, const std::string & param_name, const bool expected_value)
{
	add_bool_condition(t_name, param_name, expected_value);
}

/*
void Animator_controller::add_int_transition(const std::string & o_state_name, const std::string & d_state_name, const std::string & parameter_name, const int expected_val) 
{
	auto iter_param = m_int_parameters.find(parameter_name); // find the variable in the int parameter table 

	if (iter_param != m_int_parameters.end()) {  // if it exists
		Animation_state::pint_param parameter = &(*iter_param); // get a pointer to the <name, value> pair
		// find if the transition states exist

		auto iter_o_state = m_state_machine.find(o_state_name);
		auto iter_d_state = m_state_machine.find(d_state_name);
		if ( (iter_o_state != m_state_machine.end()) && (iter_d_state != m_state_machine.end()) ) {
			iter_o_state->second->add_int_transition(d_state_name, parameter, expected_val);
		}
	}
}

void Animator_controller::add_bool_transition(const std::string & o_state_name, const std::string & d_state_name, const std::string & parameter_name, const bool expected_val) 
{
	auto iter_param = m_bool_parameters.find(parameter_name); // find the variable in the bool parameter table 

	if (iter_param != m_bool_parameters.end()) {  // if it exists
		Animation_state::pbool_param parameter = &(*iter_param); // get a pointer to the <name, value> pair
		// find if the transition states exist
		auto iter_o_state = m_state_machine.find(o_state_name);
		auto iter_d_state = m_state_machine.find(d_state_name);
		
		if ((iter_o_state != m_state_machine.end()) && (iter_d_state != m_state_machine.end())) {
			iter_o_state->second->add_bool_transition(d_state_name, parameter, expected_val);
		}
	}
}
*/

unsigned Animator_controller::get_current_frame() const 
{
	auto iter = m_state_machine.find(m_current_state);
	if (iter != m_state_machine.end()) {
		return (iter->second).get_frame();
	}
}

Animation_state & Animator_controller::get_current_state() 
{
	auto iter = m_state_machine.find(m_current_state);
	if (iter != m_state_machine.end()) {
		return (iter->second);
	}
}

unsigned Animator_controller::get_current_anim() const 
{
	auto iter = m_state_machine.find(m_current_state);
	if (iter != m_state_machine.end()) {
		return (iter->second).get_anim();
	}
}

void Animator_controller::switch_curr_state_anim_clip(const std::vector<tgs::Animation>::size_type next_anim)
{
	auto iter = m_state_machine.find(m_current_state);
	if (iter != m_state_machine.end()) {
		return (iter->second).switch_anim_clip(next_anim);
	}
}

std::ostream & operator<<(std::ostream & os, const Animator_controller & anim_controller) 
{
	os << "---------------Animator Controller--------------------" << std::endl;
	os << "Parameters:" << std::endl;
	for (auto iter = anim_controller.m_int_parameters.cbegin(); iter != anim_controller.m_int_parameters.cend(); ++ iter) {
		os << "\t int " << iter->first << " = " << iter->second << std::endl;
	}
	for (auto iter = anim_controller.m_bool_parameters.cbegin(); iter != anim_controller.m_bool_parameters.cend(); ++iter) {
		os << "\t bool " << iter->first << " = " << iter->second << std::endl;
	}

	for (auto iter = anim_controller.m_trigger_parameters.cbegin(); iter != anim_controller.m_trigger_parameters.cend(); ++iter) {
		os << "\t trigger " << iter->first << " = " << iter->second << std::endl;
	}

	os << "States:" << std::endl;
	for (auto iter = anim_controller.m_state_machine.cbegin(); iter != anim_controller.m_state_machine.cend(); ++iter) {
		os << (iter->second);
	}
	os << "Transitions: " << std::endl;
	for (auto iter = anim_controller.m_transitions.begin(); iter != anim_controller.m_transitions.end(); ++iter) {
		os << iter->second;
	}
	os << "--------------------------------------------------------" << std::endl;
	return os;
}
/*
//THE GETTERS throw an exception.....
bool Animator_controller::get_bool_param(const std::string & name) const
{
	auto iter = m_bool_parameters.find(name);
	if (iter == m_bool_parameters.end) {
		std::cerr << __FUNCTION__ << "Unknow boolean parameter: " << name << std::endl;
		return false;
	}
	
	return iter->second;
}

bool Animator_controller::get_trigger_param(const std::string & name) const
{
	auto iter = m_trigger_parameters.find(name);
	if (iter == m_trigger_parameters.end) {
		std::cerr << __FUNCTION__ << "Unknow trigger parameter: " << name << std::endl;
		return false;
	}

	return iter->second;
}

int Animator_controller::get_int_param(const std::string & name) const
{
	auto iter = m_int_parameters.find(name);
	if (iter == m_int_parameters.end) {
		std::cerr << __FUNCTION__ << "Unknow integer parameter: " << name << std::endl;
		return 0;
	}

	return iter->second;

}*/