#include "Player_idle_state.hpp"
#include "Player_running_state.hpp"
#include "Actor.hpp"
#include "Input_handler.hpp"
#include "Timer.hpp"
#include <iostream>
#include <algorithm>

std::vector<unsigned> Player_idle_state::m_state_anim_frames;

//TODO: SWITCH ON ACTION BUTTONS, I.E. ATTACK, RUN_RIGHT, ETC
State * Player_idle_state::handle_input(const std::vector<std::pair<Button, Command *>> & stream, Actor & actor)
{
	auto iter = std::find_if(stream.begin(), stream.end(),
		[](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == KEY_A; });
	if (iter != stream.end()) {
		if ((iter->first).m_state == PRESSED) {
			std::cout << "chaging state to Player_running_state| dir= left" << std::endl;
			actor.set_facing_direction(true);     //change to running left
		    return new Player_running_state();
		}
	}

	iter = std::find_if(stream.begin(), stream.end(),
		[](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == KEY_D; });
	if (iter != stream.end()) {
		if ((iter->first).m_state == PRESSED) {
			std::cout << "Changing state to Player_running_state | dir = right" << std::endl;
			actor.set_facing_direction(false);///change to running right
			return new Player_running_state();
		}
	}
	return nullptr;
}


void Player_idle_state::update(Actor & actor) 
{
	if (!get_anim_player().is_playing()) {
		if (m_blink_timer < Timer::instance().get_time()) {
			get_anim_player().resume();
		}
	}

	//the only behavior of idle is playing the idle animation
	unsigned curr_frame = get_anim_player().get_frame();
	get_anim_player().update();
	if (curr_frame != get_anim_player().get_frame()) {
		///change texture coordinates
		//PUT THIS ON A DIFFERENT FUCTION, I.E, NOT PLAYER'S UPDATE FUNCTION, IT IS A RENDERING FUCTNIO!!!!!
		actor.get_sprite().update_uv(get_anim_player().get_frame());
		if (get_anim_player().get_frame() == 1) { //blinked
			m_blinked = true;
		}
		else if ((get_anim_player().get_frame() == 0) && ( m_blinked) ) {
			get_anim_player().pause();
			m_blink_timer = Timer::instance().get_time() + BLINK_INTERVAL;
			m_blinked = false;
		}
	}
}