#include "Player_jumping_state.hpp"
#include "Actor.hpp"
#include "AABB_2d.hpp"

#include <algorithm>

#include "Player_idle_state.hpp"
#include "Player_climbing_state.hpp"

#include "Animator_controller.hpp"
#include "Gameplay_state.hpp"
#include "vec2.hpp"

#include "Body_2d.hpp"
#include "World.hpp"
#include "Input_manager.hpp"
#include "Systems.hpp"

Player_jumping_state::Player_jumping_state(Actor & actor, float y_acceleration, float x_vel) : Gameplay_state(), m_y_acceleration(y_acceleration), m_x_vel(x_vel)
{
	actor.get_body_2d_component()->add_force(math::vec2(0.0f, m_y_acceleration));
}

Gameplay_state * Player_jumping_state::handle_input(Actor & actor) 
{
	string_id is_attacking_param_id = intern_string("is_attacking");
	string_id player_attacking_state_id = intern_string("player_attacking");
	
	bool on_ground = g_systems.m_physics_manager.get_world()->is_body_2d_on_ground(actor.get_body_2d_component());

	if (on_ground) {
		//std::cout << "changing state to player_idle" << std::endl;
		//set the paramter on the animation state machine to make the transition to the new animation
		actor.get_anim_controller_component()->set_bool("is_jumping", false);
		actor.get_body_2d_component()->stop_movement_x();
		return new Player_idle_state;
	}

	if ((actor.get_anim_controller_component()->get_current_state().get_state_id() == player_attacking_state_id)) {
		return nullptr;
	}

	const Button & climb_up_button = g_systems.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::CLIMB_UP);
	if (climb_up_button.m_state == PRESSED) {
		bool is_on_ladder = g_systems.m_physics_manager.get_world()->try_climbing_ladder(actor.get_body_2d_component(), true);
		if (is_on_ladder) {
			actor.get_anim_controller_component()->set_bool("is_jumping", false);
			actor.get_anim_controller_component()->set_bool("is_climbing", true);
			actor.get_body_2d_component()->stop_movement_x();
			actor.get_body_2d_component()->stop_movement_y();
			actor.get_body_2d_component()->apply_gravity(false);
			return new Player_climbing_state;
		}
	}

	const Button & attack_button = g_systems.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::ATTACK_01);
	if (attack_button.m_state == PRESSED) {
		//ANIMATION
		actor.get_anim_controller_component()->set_trigger("is_attacking");
		return nullptr;
	}

	const Button & move_left_button = g_systems.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_LEFT);
	if (move_left_button.m_state == PRESSED) {
		actor.get_body_2d_component()->stop_movement_x();
		actor.set_facing_direction(true);     //change to running left
		actor.get_body_2d_component()->add_force(math::vec2(-m_x_vel, 0.0f));
		//std::cout << "-----moving left here---------" << std::endl;
	}
	
	const Button & move_right_button = g_systems.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_RIGHT);
	if (move_right_button.m_state == PRESSED ) {
		actor.get_body_2d_component()->stop_movement_x();
		actor.set_facing_direction(false);     
		actor.get_body_2d_component()->add_force(math::vec2(m_x_vel, 0.0f));
		//std::cout << "-----moving left here---------" << std::endl;
	}
	
	if ((move_left_button.m_state == RELEASED) && (move_right_button.m_state == RELEASED)) {
		actor.get_body_2d_component()->stop_movement_x();
	}
	
	
	return nullptr;
}

/*void Player_jumping_state::update(Actor & actor) 
{
	actor.get_sprite().update_pos(actor.get_body_2d()->get_position(), actor.get_facing_direction());
}*/

void Player_jumping_state::begin_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << __FUNCTION__ << std::endl;
}

void Player_jumping_state::end_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << __FUNCTION__ << std::endl;
}