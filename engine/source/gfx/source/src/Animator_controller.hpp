#ifndef _ANIMATOR_CONTROLLER_HPP
#define _ANIMATOR_CONTROLLER_HPP
#include <map>
#include <iostream>
#include "string_id.hpp"
#include "Animator_controller_parameter.hpp"
#include "Animator_state.hpp"



/*  Animation_controller: This class manages a Animation state machine, stored std::map, this map associates each state name_id
 *  with a Animator_state object. Each state oject stores the transitions for that state
 *  The controller maintains a set of parameters that the state machine's transitions, use as variables for the 
 *  conditions to change from one state to another.
 */

namespace gfx { class Animation_player; struct Animator_condition; }

namespace gfx {
	class Animator_controller final {
		friend std::ostream & operator<<(std::ostream & os, const Animator_controller & anim_controller);
	public:
		Animator_controller() = default;
		
		~Animator_controller() = default;

		void update(const float dt);
		
		void set_current_state(const char *name);

		Animator_state & add_state(const char *name);
		Animator_state & add_state(const char *name, const gfx::Animation_player & anim_player);

		void			 remove_state(const Animator_state & state);
	
		void add_parameter(const char *name, const Animator_controller_parameter::Type type);
		void add_parameter(const Animator_controller_parameter & parameter);

		//Parameter setters
		void set_integer(const char *name, const int ivalue);
		void set_integer(const string_id id, const int ivalue);

		void set_bool(const char *name,   const bool bvalue);
		void set_bool(const string_id id, const bool bvalue);

		void set_trigger(const char *name);
		void set_trigger(const string_id id);
		
		//Parameters getters
		int  get_integer(const char *name)   const;
		int  get_integer(const string_id id) const;

		bool get_bool(const char *name)      const;
		bool get_bool(const string_id id)    const;

		bool get_trigger(const char *name)   const;
		bool get_trigger(const string_id id) const;

		Animator_state & get_current_state();
	private:
		bool   evaluate_condition(const Animator_controller_parameter & parameter, const Animator_condition & condition) const;

		std::map<string_id, Animator_controller_parameter>  m_parameters;
		std::map<string_id, Animator_state>				    m_state_machine;
		string_id                                           m_current_state_id;
		bool												m_param_val_changed = false;
	};

	std::ostream & operator<<(std::ostream & os, const Animator_controller & anim_controller);

}
#endif // !_ANIMATOR_CONTROLLER_HPP
