#include "Gameplay_state.hpp"
#include "Player_idle_state.hpp"
#include "Player_running_state.hpp"
#include "Player_jumping_state.hpp"
#include "Player_climbing_state.hpp"
#include "Actor.hpp"
#include "Input_manager.hpp"
#include "Button.hpp"
#include "Body_2d.hpp"
#include "AABB_2d.hpp"
#include "Timer.hpp"
#include <iostream>
#include <algorithm>
#include "game.hpp"


//TODO: SWITCH ON ACTION BUTTONS, I.E. ATTACK, RUN_RIGHT, ETC
Gameplay_state * Player_idle_state::handle_input(Actor & actor)
{
	//auto stream = Input_handler::instance().get_input();

	bool on_ground = g_physics_manager.get_world()->is_body_2d_on_ground(actor.get_body_2d());

	//auto iter = std::find_if(stream.begin(), stream.end(),
	//	[](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == KEY_A; });
	
	const Button & move_left_button = g_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_LEFT);

	if (move_left_button.m_state == PRESSED) {
		//std::cout << "chaging state to Player_running_state| dir= left" << std::endl;
		actor.set_facing_direction(true);     //change to running left
		 //set the parameter on the  animation state machine
		actor.get_sprite().get_panim_controller()->set_bool("is_running", true);
		return new Player_running_state(actor);
	}
	
	const Button & move_right_button = g_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_RIGHT);
	
	if (move_right_button.m_state == PRESSED) {
		//std::cout << "Changing state to Player_running_state | dir = right" << std::endl;
			
		//set the parameter on the animation state machine
		actor.get_sprite().get_panim_controller()->set_bool("is_running", true);

		actor.set_facing_direction(false);///change to running right
		return new Player_running_state(actor);
	}
	if (on_ground) {
		const Button & jump_button = g_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::JUMP);
		if ( jump_button.m_state == PRESSED ) {
			std::cout << "Changing to Player_jumping_state" << std::endl;
			actor.get_sprite().get_panim_controller()->set_bool("is_jumping", true);
			return new Player_jumping_state(actor);
		}
	}

	const Button & climb_up_button = g_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::CLIMB_UP);

	if (climb_up_button.m_state == PRESSED) {
		bool climbing = g_physics_manager.get_world()->try_climbing_ladder(actor.get_body_2d(), true);
		if (climbing) {
			//std::cout << "CAN CLIMB NOW!!!" << std::endl;
			//animation set up
			actor.get_sprite().get_panim_controller()->set_bool("is_climbing", true);
			actor.get_body_2d()->apply_gravity(false);
			return new Player_climbing_state();
		}
		//else {
		//	std::cout << "CANNOT CLIMB NOW :( " << std::endl;
		//}
	}

	const Button & climb_down_button = g_input_manager.get_button_from_action(Input_manager::GAME_ACTIONS::CLIMB_DOWN);
	if (climb_down_button.m_state == PRESSED) {
		bool climbing = g_physics_manager.get_world()->try_climbing_ladder(actor.get_body_2d(), false);// climb_down = tru
		if (climbing) {
			//animation set up
			actor.get_sprite().get_panim_controller()->set_bool("is_climbing", true);
			actor.get_body_2d()->apply_gravity(false);
			return new Player_climbing_state(true);
		}
	}

	return nullptr;
}

void Player_idle_state::begin_tile_collision(Actor & actor, const AABB_2d & tile_aabb) 
{
	std::cout << "PLAYER IDLE STATE " << __FUNCTION__ << ": with ";

	physics_2d::Body_2d  *pbody = actor.get_body_2d();

	if (pbody->get_aabb_2d().p_min.y >= tile_aabb.p_max.y && (pbody->get_aabb_2d().p_max.x >= tile_aabb.p_min.x && pbody->get_aabb_2d().p_min.x <= tile_aabb.p_max.x) ) {
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

void Player_idle_state::end_tile_collision(Actor & actor, const AABB_2d & tile_aabb) 
{
	std::cout << "PLAYER IDLE STATE " << __FUNCTION__ << ": with ";

	physics_2d::Body_2d  *pbody = actor.get_body_2d();

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
void Player_idle_state::update(Actor & actor) 
{
	actor.get_sprite().update_pos(actor.get_body_2d()->get_position(), actor.get_facing_direction());
	return;
}*/