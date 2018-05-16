
#include "Player_idle_state.hpp"

#include "string_id.hpp"

#include "AABB_2d.hpp"
#include "Actor.hpp"
#include "Gameplay_state.hpp"

#include "Player_ducking_state.hpp"
#include "Player_running_state.hpp"
#include "Player_jumping_state.hpp"
#include "Player_climbing_state.hpp"
#include "Animator_controller.hpp"

#include "Input_manager.hpp"
#include "Button.hpp"
#include "Body_2d.hpp"
#include "World.hpp"
#include "AABB_2d.hpp"

#include "runtime_memory_allocator.hpp"

#include <iostream>
#include <algorithm>


//TODO: SWITCH ON ACTION BUTTONS, I.E. ATTACK, RUN_RIGHT, ETC
gom::Gameplay_state * Player_idle_state::handle_input(gom::Actor & actor, Input_manager *pinput, physics_2d::World *pwld)
{
	//auto stream = Input_handler::instance().get_input();
	string_id is_attacking_param_id = intern_string("is_attacking");
	string_id player_attacking_state_id = intern_string("player_attacking");

	bool on_ground = pwld->is_body_2d_on_ground(actor.get_body_2d_component());
	//auto iter = std::find_if(stream.begin(), stream.end(),
	//	[](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == KEY_A; });
	
	if ((actor.get_anim_controller_component()->get_current_state().get_state_id() == player_attacking_state_id)) { //still playing attacking animation, cant move
		return nullptr;
	}

	const Button & move_left_button = pinput->get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_LEFT);

	if (move_left_button.m_state == PRESSED) {
		//std::cout << "chaging state to Player_running_state| dir= left" << std::endl;
		actor.set_facing_direction(true);     //change to running left
		 //set the parameter on the  animation state machine
		actor.get_anim_controller_component()->set_bool("is_running", true);
		
		
		
		void *pmem = mem::allocate(sizeof(Player_running_state));
		return static_cast<gom::Gameplay_state*> (new (pmem) Player_running_state(actor));
	}
	
	const Button & move_right_button = pinput->get_button_from_action(Input_manager::GAME_ACTIONS::MOVE_RIGHT);
	
	if (move_right_button.m_state == PRESSED) {
		//std::cout << "Changing state to Player_running_state | dir = right" << std::endl;
			
		//set the parameter on the animation state machine
		actor.get_anim_controller_component()->set_bool("is_running", true);

		actor.set_facing_direction(false);///change to running right
		
		void *pmem = mem::allocate(sizeof(Player_running_state));
		return static_cast<gom::Gameplay_state*> (new (pmem) Player_running_state(actor));
	}
	if (on_ground) {
		const Button & jump_button = pinput->get_button_from_action(Input_manager::GAME_ACTIONS::JUMP);
		if ( jump_button.m_state == PRESSED ) {
			std::cout << "Changing to Player_jumping_state" << std::endl;
			actor.get_anim_controller_component()->set_bool("is_jumping", true);
			
			void *pmem = mem::allocate(sizeof(Player_jumping_state));
			gom::Gameplay_state *pstate = static_cast<gom::Gameplay_state*> (new (pmem) Player_jumping_state(actor));
			return pstate;
			
		}
	}

	const Button & climb_up_button = pinput->get_button_from_action(Input_manager::GAME_ACTIONS::CLIMB_UP);

	if (climb_up_button.m_state == PRESSED) {
		bool climbing = pwld->try_climbing_ladder(actor.get_body_2d_component(), true);
		if (climbing) {
			//std::cout << "CAN CLIMB NOW!!!" << std::endl;
			//animation set up
			actor.get_anim_controller_component()->set_bool("is_climbing", true);
			actor.get_body_2d_component()->apply_gravity(false);
			
			void *pmem = mem::allocate(sizeof(Player_climbing_state));
			return static_cast<gom::Gameplay_state*> (new (pmem) Player_climbing_state);
			//return new Player_climbing_state();
		}
		//else {
		//	std::cout << "CANNOT CLIMB NOW :( " << std::endl;
		//}
	}

	const Button & climb_down_button = pinput->get_button_from_action(Input_manager::GAME_ACTIONS::CLIMB_DOWN);
	if (climb_down_button.m_state == PRESSED) {
		bool climbing = pwld->try_climbing_ladder(actor.get_body_2d_component(), false);// climb_down = tru
		if (climbing) {
			//animation set up
			actor.get_anim_controller_component()->set_bool("is_climbing", true);
			actor.get_body_2d_component()->apply_gravity(false);
			
			void *pmem = mem::allocate(sizeof(Player_climbing_state));
			return static_cast<gom::Gameplay_state*> (new (pmem) Player_climbing_state(true));
			//return new Player_climbing_state(true);
		}
		else { // no ladder, change to ducking state
			//animation
			actor.get_anim_controller_component()->set_bool("is_ducking", true);
			//gameplay
			
			void *pmem = mem::allocate(sizeof(Player_ducking_state));
			return static_cast<gom::Gameplay_state*> (new (pmem) Player_ducking_state);

			//return new Player_ducking_state;
		}

	}

	const Button & attacking_button = pinput->get_button_from_action(Input_manager::GAME_ACTIONS::ATTACK_01);
	if ( (attacking_button.m_state == PRESSED) ) {
		actor.get_anim_controller_component()->set_trigger(is_attacking_param_id);
	}

	return nullptr;
}

size_t Player_idle_state::get_size() const 
{
	return sizeof(Player_idle_state);
}

void Player_idle_state::begin_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << "PLAYER IDLE STATE " << __FUNCTION__ << ": with ";

	physics_2d::Body_2d  *pbody = actor.get_body_2d_component();

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

void Player_idle_state::end_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << "PLAYER IDLE STATE " << __FUNCTION__ << ": with ";

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
void Player_idle_state::update(Actor & actor) 
{
	actor.get_sprite().update_pos(actor.get_body_2d()->get_position(), actor.get_facing_direction());
	return;
}*/