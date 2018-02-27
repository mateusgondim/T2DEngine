#include "Gameplay_state.hpp"
#include "Player_running_state.hpp"
#include "Player_idle_state.hpp"
#include "Player_jumping_state.hpp"
#include "Animation_player.hpp"
#include "Timer.hpp"
#include "Actor.hpp"
#include "Body_2d.hpp"
#include "Input_manager.hpp"
#include "Engine.hpp"
#include <iostream>
#include <algorithm>


Player_running_state::Player_running_state(Actor & actor, const float acceleration) : Gameplay_state(), m_acceleration(acceleration) 
{
	if (actor.get_facing_direction()) {
		actor.get_body_2d_component()->add_force(math::vec2(-acceleration, 0.0f));
	}
	else {
		actor.get_body_2d_component()->add_force(math::vec2(acceleration, 0.0f));
	}
}

Gameplay_state * Player_running_state::handle_input(Actor & actor)
{
	//auto stream = Input_handler::instance().get_input();
	bool on_ground = g_engine.m_physics_manager.get_world()->is_body_2d_on_ground(actor.get_body_2d_component());
	
	if (!on_ground) { //player fell
		actor.get_anim_controller_component()->set_bool("is_running", false);
		actor.get_anim_controller_component()->set_bool("is_jumping", true);
		//actor.get_body_2d()->stop_movement_x();
		return new Player_jumping_state(actor, 0.0f);
	}
	else {
		if (actor.get_facing_direction()) {
			const Button & move_left_button = g_engine.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_LEFT);
			if (move_left_button.m_state == RELEASED) {
				//std::cout << "chaging state to Player_idle" << std::endl;
				//set the paramter on the animation state machine to make the transition to the new animation
				actor.get_anim_controller_component()->set_bool("is_running", false);
				actor.get_body_2d_component()->stop_movement_x();
				return new Player_idle_state;
			}
		}
		else {
			const Button & move_right_button = g_engine.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_RIGHT);
			if (move_right_button.m_state == RELEASED) {
				//std::cout << "changing state to player_idle" << std::endl;
				//set the paramter on the animation state machine to make the transition to the new animation
				actor.get_anim_controller_component()->set_bool("is_running", false);
				actor.get_body_2d_component()->stop_movement_x();
				return new Player_idle_state;
			}
		}

		const Button & jump_button = g_engine.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::JUMP);
		if ((jump_button.m_state == PRESSED) && on_ground) {
			//std::cout << "Changing  to player_jumping_state " << std::endl;
			//set the paramter on the animation state machine to make the transition to the new animation
			actor.get_anim_controller_component()->set_bool("is_running", false);
			actor.get_anim_controller_component()->set_bool("is_jumping", true);
			//actor.get_body_2d()->stop_movement_x();
			return new Player_jumping_state(actor);
		}
		const Button & attack_button = g_engine.m_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::ATTACK_01);
		if (attack_button.m_state == PRESSED) {
			//ANIMATION
			actor.get_anim_controller_component()->set_bool("is_running", false);
			actor.get_anim_controller_component()->set_trigger("is_attacking");
			
			//GAMEPLAY
			actor.get_body_2d_component()->stop_movement_x();
			//launch projectile

			return new Player_idle_state;
			
		}
	}
	return nullptr;
}
void Player_running_state::begin_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << "PLAYER RUNNING STATE " << __FUNCTION__ << ": with ";

	physics_2d::Body_2d  *pbody = actor.get_body_2d_component();

	if (pbody->get_aabb_2d().p_min.y >= tile_aabb.p_max.y && (pbody->get_aabb_2d().p_max.x >= tile_aabb.p_min.x && pbody->get_aabb_2d().p_min.x <= tile_aabb.p_max.x)) {
		std::cout << "Floor tile" << std::endl;
	}
	else if (pbody->get_aabb_2d().p_max.y <= tile_aabb.p_min.y && (pbody->get_aabb_2d().p_max.x >= tile_aabb.p_min.x && pbody->get_aabb_2d().p_min.x <= tile_aabb.p_max.x)) {
		std::cout << "Ceiling tile" << std::endl;
	}
	else if (pbody->get_aabb_2d().p_max.x >= tile_aabb.p_min.x && (pbody->get_aabb_2d().p_min.y < tile_aabb.p_max.y && pbody->get_aabb_2d().p_max.y > tile_aabb.p_min.y)) {
		std::cout << "Right wall tile" << std::endl;
	}
	else if (pbody->get_aabb_2d().p_min.x <= tile_aabb.p_max.x && (pbody->get_aabb_2d().p_min.y < tile_aabb.p_max.y && pbody->get_aabb_2d().p_max.y > tile_aabb.p_min.y)) {
		std::cout << "Left wall tile " << std::endl;
	}
	else {
		std::cerr << " unknow tile aabb coordinate..." << std::endl;
	}
}
void Player_running_state::end_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << "PLAYER RUNNING STATE " << __FUNCTION__ << ": with ";

	physics_2d::Body_2d  *pbody = actor.get_body_2d_component();

	if (pbody->get_aabb_2d().p_min.y >= tile_aabb.p_max.y && (pbody->get_aabb_2d().p_max.x >= tile_aabb.p_min.x && pbody->get_aabb_2d().p_min.x <= tile_aabb.p_max.x)) {
		std::cout << "Floor tile" << std::endl;
	}
	else if (pbody->get_aabb_2d().p_max.y <= tile_aabb.p_min.y && (pbody->get_aabb_2d().p_max.x >= tile_aabb.p_min.x && pbody->get_aabb_2d().p_min.x <= tile_aabb.p_max.x)) {
		std::cout << "Ceiling tile" << std::endl;
	}
	else if (pbody->get_aabb_2d().p_max.x >= tile_aabb.p_min.x && (pbody->get_aabb_2d().p_min.y < tile_aabb.p_max.y && pbody->get_aabb_2d().p_max.y > tile_aabb.p_min.y)) {
		std::cout << "Right wall tile" << std::endl;
	}
	else if (pbody->get_aabb_2d().p_min.x <= tile_aabb.p_max.x && (pbody->get_aabb_2d().p_min.y < tile_aabb.p_max.y && pbody->get_aabb_2d().p_max.y > tile_aabb.p_min.y)) {
		std::cout << "Left wall tile " << std::endl;
	}
	else {
		std::cerr << " unknow tile aabb coordinate..." << std::endl;
	}
}

/*
void Player_running_state::update(Actor & actor) 
{
	actor.get_sprite().update_pos(actor.get_body_2d()->get_position(), actor.get_facing_direction());
}*/