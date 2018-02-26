#ifndef _ANIMATION_STATE_HPP
#define _ANIMATION_STATE_HPP	

#include <string>
#include <iostream>
#include "Animation_player.hpp"


/* Animation_state - Class to represent a state on the animation state machine
 * each state has a animation_player so it can play the animation on its update() function.
 * It also has a map for each transition using int, bool or trigger parameters. The maps stores for each transition, 
   the name of the destination state as the key and, as the value, a pair containig a pointer to the parameter name and value and,
   the value of the parameters that is expected to make the transition
 */
namespace gfx {
	class Animation_state {
		friend std::ostream & operator<<(std::ostream & os, const Animation_state & anim_state);
	public:
		//typedef const std::pair<const std::string, int>  * pint_param;
		//typedef const std::pair<const std::string, bool> * pbool_param;
		//typedef pbool_param                          ptrigger_param;

		Animation_state(const std::string & name, const gfx::Animation_player & anim_player) : m_name(name), m_animation_player(anim_player) {}

		//copy control
		//Animation_state(const Animation_state &anim_state); // 
		//Animation_state & operator=(const Animation_state & rhs);

		//void add_int_transition(const std::string & state_name, pint_param parameter, const int expected_int) { m_int_transitions.insert({ state_name,{ parameter, expected_int } }); };
		//void add_bool_transition(const std::string & state_name, pbool_param parameter, const bool expected_bool) { m_bool_transitions.insert( { state_name, {parameter, expected_bool} }); }
		//void add_trigger_transition(const std::string & state_name, ptrigger_param parameter) { m_trigger_transitions.insert({ state_name, parameter }); };

		void update(const float dt);
		//std::string check_transition(); //return the name of the state to transition
		void switch_anim_clip(const std::vector<gfx::Animation>::size_type next_anim) { m_animation_player.switch_animation(next_anim); }

		void stop_animation() { m_animation_player.pause(); }
		void start_animation() { m_animation_player.start_from_beg(); }
		std::string get_name() const { return m_name; }
		//tgs::Animation_player & get_anim_player() { return m_animation_player; }
		unsigned get_frame() const { return m_animation_player.get_frame(); }
		unsigned get_anim()  const { return m_animation_player.get_anim(); }
		gfx::Animation_player & get_anim_player() { return m_animation_player; }
	private:
		//void clear_transitions();

		std::string											  m_name;
		gfx::Animation_player								  m_animation_player;
		//std::map<std::string, std::pair<pint_param,   int>>   m_int_transitions;
		//std::map<std::string, std::pair<pbool_param,  bool>>  m_bool_transitions;
		//std::map<std::string, ptrigger_param>				  m_trigger_transitions;

	};

	bool operator==(const Animation_state & lhs, const Animation_state & rhs);
	bool operator!=(const Animation_state & lhs, const Animation_state & rhs);

	std::ostream & operator<<(std::ostream & os, const Animation_state & anim_state);

}
#endif // !_ANIMATION_STATE_HPP
