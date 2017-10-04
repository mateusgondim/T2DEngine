#include "Gameplay_state.hpp"
#include "Player_idle_state.hpp"
#include "Player_running_state.hpp"
#include "Actor.hpp"
#include "Input_handler.hpp"
#include "Button.hpp"
#include "Command.hpp"
#include "Timer.hpp"
#include <iostream>
#include <algorithm>


//TODO: SWITCH ON ACTION BUTTONS, I.E. ATTACK, RUN_RIGHT, ETC
Gameplay_state * Player_idle_state::check_transition(Actor & actor)
{
	auto stream = Input_handler::instance().get_input();

	auto iter = std::find_if(stream.begin(), stream.end(),
		[](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == KEY_A; });
	if (iter != stream.end()) {
		if ((iter->first).m_state == PRESSED) {
			std::cout << "chaging state to Player_running_state| dir= left" << std::endl;
			actor.set_facing_direction(true);     //change to running left
		    //set the parameter on the  animation state machine
			actor.get_sprite().get_panim_controller()->set_bool("is_running", true);
			return new Player_running_state();
		}
	}

	iter = std::find_if(stream.begin(), stream.end(),
		[](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == KEY_D; });
	if (iter != stream.end()) {
		if ((iter->first).m_state == PRESSED) {
			std::cout << "Changing state to Player_running_state | dir = right" << std::endl;
			
			//set the parameter on the animation state machine
			actor.get_sprite().get_panim_controller()->set_bool("is_running", true);

			actor.set_facing_direction(false);///change to running right
			return new Player_running_state();
		}
	}
	return nullptr;
}


void Player_idle_state::update(Actor & actor) 
{
	//if (!get_anim_player().is_playing()) {
	//	if (m_blink_timer < Timer::instance().get_time()) {
	//		get_anim_player().resume();
	//	}
	//}

	//the only behavior of idle is playing the idle animation
//	unsigned curr_frame = get_anim_player().get_frame();
//	get_anim_player().update();
//	if (curr_frame != get_anim_player().get_frame()) {
		///change texture coordinates
		//PUT THIS ON A DIFFERENT FUCTION, I.E, NOT PLAYER'S UPDATE FUNCTION, IT IS A RENDERING FUCTNIO!!!!!
	//	actor.get_sprite().update_uv(get_anim_player().get_frame());
	//	if (get_anim_player().get_frame() == 1) { //blinked
	//		m_blinked = true;
	//	}
//		else if ((get_anim_player().get_frame() == 0) && ( m_blinked) ) {
//			get_anim_player().pause();
//			m_blink_timer = Timer::instance().get_time() + BLINK_INTERVAL;
//			m_blinked = false;
//		}
//	}
	return;
}