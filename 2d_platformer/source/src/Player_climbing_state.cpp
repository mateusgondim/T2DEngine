#include "Player_climbing_state.hpp"
#include "Player_idle_state.hpp"
#include "Player_jumping_state.hpp"
#include "Actor.hpp"
#include "AABB_2d.hpp"
#include "Body_2d.hpp"
#include "Button.hpp"
#include "Input_manager.hpp"
#include "game.hpp"

Player_climbing_state::Player_climbing_state(const cgm::vec2 & climbing_vel) : Gameplay_state(), m_climbing_vel(climbing_vel) {}

Gameplay_state * Player_climbing_state::check_transition(Actor & actor) 
{
	bool is_on_ladder = g_physics_manager.get_world()->is_body_on_ladder(actor.get_body_2d());
	
	const Button & moving_left_button = g_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_LEFT);
	if (moving_left_button.m_state == PRESSED) {
		actor.get_body_2d()->stop_movement_y();
		actor.get_body_2d()->apply_gravity(true);
		
		actor.set_facing_direction(true);

		actor.get_sprite().get_panim_controller()->set_bool("is_jumping", true);
		return new Player_jumping_state(actor, 0.0f);
	}

	const Button & moving_right_button = g_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_RIGHT);
	if (moving_right_button.m_state == PRESSED) {
		actor.get_body_2d()->stop_movement_y();
		actor.get_body_2d()->apply_gravity(true);

		actor.set_facing_direction(false);

		actor.get_sprite().get_panim_controller()->set_bool("is_jumping", true);
		return new Player_jumping_state(actor,0.0f);
	}

	if (is_on_ladder) {
		std::cout << __FUNCTION__ << ": INSIDE LADDER" << std::endl;
		const Button & moving_up_button = g_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_UP);
		if (moving_up_button.m_state == PRESSED) {
			actor.get_body_2d()->set_velocity(m_climbing_vel);
		}
		else {
			const Button & moving_down_button = g_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_DOWN);
			if (moving_down_button.m_state == PRESSED) {
				actor.get_body_2d()->set_velocity(-m_climbing_vel);
			}
			else if (moving_down_button.m_state == RELEASED && moving_up_button.m_state == RELEASED) {
				actor.get_body_2d()->stop_movement_y();
			}
		}
		return nullptr;
	}
	else {//OUT OF THE LADDER
		std::cout << "-----------------out of ladder---------- " << std::endl;
		actor.get_body_2d()->apply_gravity(true);
		return new Player_idle_state();
	}
}

void Player_climbing_state::update(Actor & actor) 
{
	actor.get_sprite().update_pos(actor.get_body_2d()->get_position(), actor.get_facing_direction());
	return;
}


void Player_climbing_state::begin_tile_collision(Actor & actor, const AABB_2d & tile_aabb)
{
	std::cout << __FUNCTION__ << std::endl;
}

void Player_climbing_state::end_tile_collision(Actor & actor, const AABB_2d & tile_aabb)
{
	std::cout << __FUNCTION__ << std::endl;
}