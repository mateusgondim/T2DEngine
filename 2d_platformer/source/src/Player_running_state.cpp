#include "Gameplay_state.hpp"
#include "Player_running_state.hpp"
#include "Player_idle_state.hpp"
#include "Animation_player.hpp"
#include "Timer.hpp"
#include "Actor.hpp"
#include "Input_handler.hpp"
#include <iostream>
#include <algorithm>


Gameplay_state * Player_running_state::check_transition(Actor & actor)
{
	auto stream = Input_handler::instance().get_input();

	if (actor.get_facing_direction()) {
		auto iter = std::find_if(stream.begin(), stream.end(),
			[](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == KEY_A; });
		if (iter != stream.end()) {
			if ((iter->first).m_state == RELEASED) {
				std::cout << "chaging state to Player_idle" << std::endl;
				//set the paramter on the animation state machine to make the transition to the new animation
				actor.get_sprite().get_panim_controller()->set_bool("is_running", false);

				return new Player_idle_state;
			}
		}
	}
	else {
		auto iter = std::find_if(stream.begin(), stream.end(),
			[](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == KEY_D; });
		if (iter != stream.end()) {
			if ((iter->first).m_state == RELEASED) {
				std::cout << "changing state to player_idle" << std::endl;
				//set the paramter on the animation state machine to make the transition to the new animation
				actor.get_sprite().get_panim_controller()->set_bool("is_running", false);

				return new Player_idle_state;
			}
		}
	}
	return nullptr;
}

void Player_running_state::update(Actor & actor) 
{
	cgm::vec2 & vel = actor.get_velocity();

	//update the position accordly to the running direction
	if (actor.get_facing_direction()) { // true if is facing left
		actor.get_position() += cgm::vec3(-(vel.x * Timer::instance().get_delta()), 0.0f, 0.0f);
	}
	else {
		actor.get_position() += cgm::vec3(vel.x * Timer::instance().get_delta(), 0.0f, 0.0f);
	}

	actor.get_sprite().update_pos(actor.get_position(), actor.get_facing_direction());
}