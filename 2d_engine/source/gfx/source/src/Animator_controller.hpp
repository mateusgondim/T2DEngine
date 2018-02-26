#ifndef _ANIMATOR_CONTROLLER_HPP
#define _ANIMATOR_CONTROLLER_HPP
#include <map>
#include <iostream>
#include <memory>
#include "Animation_state.hpp"
#include "Transition.hpp"

/* Animation_controller: This class manages a Animation state machine, it state
 * contains a animation and is stored in the map m_state_machine, the transitions,
 * witch are stored in the m_transitions, contain the conditions that need to hold for the
 * state machine to transition from one state to another. The controller maintains a set of parameters
 * that the state machine uses for the conditions, 
 */

namespace gfx { class Animation_state; }

namespace gfx {
	class Animator_controller {
		friend std::ostream & operator<<(std::ostream & os, const Animator_controller & anim_controller);
	public:
		Animator_controller() = default;

		void update(const float dt);
		void add_state(const Animation_state & anim_state);
		void set_initial_state(const std::string & name);

		void add_transition(const Transition & transition) { m_transitions.insert({ transition.get_name(), transition }); }

		void add_int_condition(const std::string & t_name, const std::string & param_name, Condition::Cond_operator op, const int expected_value);
		void add_bool_condition(const std::string & t_name, const std::string & param_name, const bool expected_value);
		void add_trigger_condition(const std::string & t_name, const std::string & param_name, const bool expected_value);

		//add transition parameters
		void add_trigger_param(const std::string & name, const bool value = false) { m_trigger_parameters.insert({ name, value }); }
		void add_int_param(const std::string  & name, const int value) { m_int_parameters.insert({ name, value }); }
		void add_bool_param(const std::string & name, const bool value = false) { m_bool_parameters.insert({ name, value }); }

		//void add_int_transition(const std::string & o_state_name, const std::string & d_state_name,  const std::string & parameter_name, const int expected_val);
		//void add_bool_transition(const std::string & o_state_name, const std::string & d_state_name, const std::string & parameter_name, const bool expected_val);
		//void add_trigger_transition(const std::string & o_state_name, const std::string & d_state_name, const std::string & parameter_name);

		//setters
		void set_int(const std::string & name, const int ivalue);
		void set_bool(const std::string & name, const bool bvalue);
		void set_trigger(const std::string & name);
		bool get_trigger(const std::string & name) const;

		//getters
		//bool get_bool_param(const std::string & name)     const;
		//int  get_int_param(const std::string & name)      const;
		//bool get_trigger_param(const std::string & name)  const;
		Animation_state & get_current_state();
		unsigned get_current_frame() const;
		unsigned get_current_anim() const;
		void     switch_curr_state_anim_clip(const std::vector<gfx::Animation>::size_type next_anim);
	private:
		std::map<std::string, Transition>       m_transitions;
		std::map<std::string, Animation_state>  m_state_machine;
		std::map<std::string, bool>				m_trigger_parameters;
		std::map<std::string, bool>				m_bool_parameters;
		std::map<std::string, int>				m_int_parameters;
		std::string                             m_current_state;
		std::string								m_initial_state;
		bool								    m_param_val_changed = false;
		bool                                    m_pending_trigger = false;
	};

	std::ostream & operator<<(std::ostream & os, const Animator_controller & anim_controller);

}
#endif // !_ANIMATOR_CONTROLLER_HPP
