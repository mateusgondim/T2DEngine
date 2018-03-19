#include "Player_ducking_state.hpp"

#include "AABB_2d.hpp"
#include "Actor.hpp"
#include "Gameplay_state.hpp"
#include "Animator_controller.hpp"
#include "Engine.hpp"

#include "Player_idle_state.hpp"

Gameplay_state * Player_ducking_state::handle_input(Actor & actor) 
{
	//check if is attacking, if so, return
	bool is_attacking = actor.get_anim_controller_component()->get_trigger("is_attacking");
	if (is_attacking) {
		return nullptr;
	}

	//check if released ducking button, if so, return to idle
	const Button & move_down_button = g_engine.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_DOWN);
	if (move_down_button.m_state == RELEASED) {
		//ANIMATION
		actor.get_anim_controller_component()->set_bool("is_ducking", false);
		//Gameplay
		return new Player_idle_state;
	}

	//check if pressed left or right, if so, change orientation
	const Button & move_left_button   =  g_engine.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_LEFT);
	if (move_left_button.m_state == PRESSED) {
		//change orientation
		actor.set_facing_direction(true);
		return nullptr;
	}
	else { 
		const Button & move_right_button = g_engine.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_RIGHT);
		if (move_right_button.m_state == PRESSED) {
			actor.set_facing_direction(false);
			return nullptr;
		}
	}
	
	//check if pressed attack button, if so, attack
	const Button & attacking_button = g_engine.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::ATTACK_01);

	if (attacking_button.m_state == PRESSED) {
		//ANIMATION
		actor.get_anim_controller_component()->set_trigger("is_attacking");
		//gameplay
		//throw projectile...
	}
	return nullptr;
}

void Player_ducking_state::begin_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	;
}

void Player_ducking_state::end_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	;
}