#include "Player_climbing_state.hpp"
#include "Player_idle_state.hpp"
#include "Player_jumping_state.hpp"
#include "Actor.hpp"
#include "AABB_2d.hpp"
#include "Body_2d.hpp"
#include "Button.hpp"
#include "Input_manager.hpp"
#include "Engine.hpp"

Player_climbing_state::Player_climbing_state(const bool climbing_from_top, const math::vec2 & climbing_vel) :
	Gameplay_state(), m_from_top(climbing_from_top), m_climbing_vel(climbing_vel), m_anim_clip(0) {}

Gameplay_state * Player_climbing_state::handle_input(Actor & actor)
{
	math::Rect bounds;
	
	bool is_on_ladder = g_engine.m_physics_manager.get_world()->is_body_on_ladder(actor.get_body_2d_component());
	bool is_on_ladder_top = g_engine.m_physics_manager.get_world()->is_on_ladder_top_tile(actor.get_body_2d_component(), bounds);
	bool is_on_ground = g_engine.m_physics_manager.get_world()->is_body_2d_on_ground(actor.get_body_2d_component());
	//climbing from top animation and small displacement
	if (m_from_top) {
		if (m_anim_clip == 0) {
			actor.get_anim_controller_component()->switch_curr_state_anim_clip(1);
			actor.get_body_2d_component()->set_velocity(-m_climbing_vel);
		}
		if (bounds.y > actor.get_body_2d_component()->get_aabb_2d().p_max.y) {
			actor.get_body_2d_component()->stop_movement_y();
			actor.get_anim_controller_component()->switch_curr_state_anim_clip(0);
			m_from_top = false;
		}
		return nullptr;
	}
	else {
		if (!m_reached_top) { //in the last frame it was still climbing the ladder 
			
			if (!is_on_ladder) { // have fallen of
				actor.get_body_2d_component()->stop_movement_y();
				actor.get_body_2d_component()->apply_gravity(true);

				actor.set_facing_direction(true);
				actor.get_anim_controller_component()->set_bool("is_climbing", false);
				actor.get_anim_controller_component()->set_bool("is_jumping", true);
				return new Player_jumping_state(actor, 0.0f);
			}

			const Button & moving_left_button = g_engine.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_LEFT);
			if (moving_left_button.m_state == PRESSED) {
				actor.get_body_2d_component()->stop_movement_y();
				actor.get_body_2d_component()->apply_gravity(true);

				actor.set_facing_direction(true);
				actor.get_anim_controller_component()->set_bool("is_climbing", false);
				actor.get_anim_controller_component()->set_bool("is_jumping", true);
				return new Player_jumping_state(actor, 0.0f);
			}

			const Button & moving_right_button = g_engine.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_RIGHT);
			if (moving_right_button.m_state == PRESSED) {
				actor.get_body_2d_component()->stop_movement_y();
				actor.get_body_2d_component()->apply_gravity(true);

				actor.set_facing_direction(false);

				actor.get_anim_controller_component()->set_bool("is_climbing", false);
				actor.get_anim_controller_component()->set_bool("is_jumping", true);
				return new Player_jumping_state(actor, 0.0f);
			}


			//std::cout << __FUNCTION__ << ": INSIDE LADDER" << std::endl;
			const Button & moving_up_button = g_engine.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_UP);
			if (moving_up_button.m_state == PRESSED) {
				actor.get_body_2d_component()->set_velocity(m_climbing_vel);
				actor.get_anim_controller_component()->get_current_state().get_anim_player().resume();
			}
			else {
				const Button & moving_down_button = g_engine.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_DOWN);
				if (is_on_ground) {
					actor.get_body_2d_component()->stop_movement_y();
					actor.get_body_2d_component()->apply_gravity(true);
					actor.get_anim_controller_component()->set_bool("is_climbing", false);
					return new Player_idle_state();
				}

				if (moving_down_button.m_state == PRESSED) {
					actor.get_body_2d_component()->set_velocity(-m_climbing_vel);
					actor.get_anim_controller_component()->get_current_state().get_anim_player().resume();
				}
				else if (moving_down_button.m_state == RELEASED && moving_up_button.m_state == RELEASED) {
					actor.get_body_2d_component()->stop_movement_y();
					actor.get_anim_controller_component()->get_current_state().get_anim_player().pause();
				}
			}

			if (is_on_ladder_top) {//REACHED THE TOP
				//std::cout << "-----------------reached top of ladder---------- " << std::endl;
				if (actor.get_body_2d_component()->get_velocity().y > 0.0f && (bounds.y <= actor.get_body_2d_component()->get_aabb_2d().p_max.y)) {
					m_reached_top = true;
					actor.get_body_2d_component()->set_velocity(math::vec2(0.0f, m_climbing_vel.y));
					//start climbing off animation
					actor.get_anim_controller_component()->switch_curr_state_anim_clip(1);
					return nullptr;
				}
				else {
					return nullptr;
				}
			}
			return nullptr;
		}
		else { //climbing off from the top, one tile
			float center_y = (actor.get_body_2d_component()->get_aabb_2d().p_max.y + actor.get_body_2d_component()->get_aabb_2d().p_min.y) / 2.0f;
			if (center_y > bounds.y) {
				float y_displacement = bounds.y - actor.get_body_2d_component()->get_aabb_2d().p_min.y;

				actor.get_body_2d_component()->get_position().y += y_displacement;		// + FLOAT_ROUNDOFF;
				actor.get_body_2d_component()->get_aabb_2d().p_max.y += y_displacement;	// +FLOAT_ROUNDOFF;
				actor.get_body_2d_component()->get_aabb_2d().p_min.y += y_displacement;	// +FLOAT_ROUNDOFF;

				actor.get_body_2d_component()->stop_movement_y();
				actor.get_body_2d_component()->apply_gravity(true);
				actor.get_anim_controller_component()->set_bool("is_climbing", false);
				actor.get_anim_controller_component()->switch_curr_state_anim_clip(0);
				return new Player_idle_state();
			}
			return nullptr;
		}
	}
}
/*
void Player_climbing_state::update(Actor & actor) 
{
	actor.get_sprite().update_pos(actor.get_body_2d()->get_position(), actor.get_facing_direction());
	return;
}*/


void Player_climbing_state::begin_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << __FUNCTION__ << std::endl;
}

void Player_climbing_state::end_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << __FUNCTION__ << std::endl;
}