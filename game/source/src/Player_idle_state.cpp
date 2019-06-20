
#include "Player_idle_state.hpp"

#include "string_id.hpp"

#include "vec2.hpp"

#include "AABB_2d.hpp"
#include "Physics_manager.hpp"
#include "Actor.hpp"
#include "Gameplay_state.hpp"
#include "Game_object_handle.hpp"

#include "Player.hpp"
#include "Player_taking_hit_state.hpp"
#include "Player_ducking_state.hpp"
#include "Player_running_state.hpp"
#include "Player_jumping_state.hpp"
#include "Player_climbing_state.hpp"
#include "Animator_controller.hpp"

#include "Input_manager.hpp"
#include "crc32.hpp"
#include "Body_2d.hpp"
#include "World.hpp"
#include "AABB_2d.hpp"

#include "Projectile_manager.hpp"
#include "Game_object_manager.hpp"

#include "runtime_memory_allocator.hpp"

#include <iostream>
#include <algorithm>


//TODO: SWITCH ON ACTION BUTTONS, I.E. ATTACK, RUN_RIGHT, ETC
gom::Gameplay_state * Player_idle_state::handle_input(gom::Actor & actor)
{
	//auto stream = Input_handler::instance().get_input();
	string_id is_attacking_param_id = intern_string("is_attacking");
	string_id player_attacking_state_id = intern_string("player_attacking");
	string_id knife_obj_id				= intern_string("knife_obj");

    //check if is taking a hit
    Player *pplayer = static_cast<Player*>(&actor);
    if (pplayer->is_taking_hit()) {
            actor.get_anim_controller_component()->set_bool("is_taking_hit", true);
            void *pmem = mem::allocate(sizeof(Player_taking_hit_state));
            return static_cast<gom::Gameplay_state*> (new (pmem) Player_taking_hit_state(actor));
    }

	bool on_ground = physics_2d::g_physics_mgr.get_world()->is_body_2d_on_ground(actor.get_body_2d_component());
		
	if ((actor.get_anim_controller_component()->get_current_state().get_state_id() == player_attacking_state_id)) { //still playing attacking animation, cant move
		return nullptr;
	}

    bool is_move_left_pressed = io::g_input_mgr.get_button(SID('move_left'));

	if (is_move_left_pressed) {
		//std::cout << "chaging state to Player_running_state| dir= left" << std::endl;
		actor.set_facing_direction(true);     //change to running left
		 //set the parameter on the  animation state machine
		actor.get_anim_controller_component()->set_bool("is_running", true);
		
		
		
		void *pmem = mem::allocate(sizeof(Player_running_state));
		return static_cast<gom::Gameplay_state*> (new (pmem) Player_running_state(actor));
	}
	
    bool is_move_right_pressed = io::g_input_mgr.get_button(SID('move_right'));
	
	if (is_move_right_pressed) {
		//std::cout << "Changing state to Player_running_state | dir = right" << std::endl;
			
		//set the parameter on the animation state machine
		actor.get_anim_controller_component()->set_bool("is_running", true);

		actor.set_facing_direction(false);///change to running right
		
		void *pmem = mem::allocate(sizeof(Player_running_state));
		return static_cast<gom::Gameplay_state*> (new (pmem) Player_running_state(actor));
	}
	if (on_ground) {
        bool is_jump_pressed = io::g_input_mgr.get_button(SID('jump'));
		if (is_jump_pressed) {
			actor.get_anim_controller_component()->set_bool("is_jumping", true);
			
			void *pmem = mem::allocate(sizeof(Player_jumping_state));
			gom::Gameplay_state *pstate = static_cast<gom::Gameplay_state*> (new (pmem) Player_jumping_state(actor));
			return pstate;
			
		}
	}

    bool is_climb_up_pressed = io::g_input_mgr.get_button_down(SID('climb_up'));
	if (is_climb_up_pressed) {
		bool climbing = physics_2d::g_physics_mgr.get_world()->try_climbing_ladder(actor.get_body_2d_component(), true);
		if (climbing) {
			//std::cout << "CAN CLIMB NOW!!!" << std::endl;
			//animation set up
			actor.get_anim_controller_component()->set_bool("is_climbing", true);
			actor.get_body_2d_component()->set_gravity_scale(0.0f);
			
			void *pmem = mem::allocate(sizeof(Player_climbing_state));
			return static_cast<gom::Gameplay_state*> (new (pmem) Player_climbing_state);
			//return new Player_climbing_state();
		}
		//else {
		//	std::cout << "CANNOT CLIMB NOW :( " << std::endl;
		//}
	}

    bool is_climb_down_pressed = io::g_input_mgr.get_button_down(SID('climb_down'));
	if (is_climb_down_pressed) {
		bool climbing = physics_2d::g_physics_mgr.get_world()->try_climbing_ladder(actor.get_body_2d_component(), false);// climb_down = tru
		if (climbing) {
			//animation set up
			actor.get_anim_controller_component()->set_bool("is_climbing", true);
			actor.get_body_2d_component()->set_gravity_scale(0.0f);
			
			void *pmem = mem::allocate(sizeof(Player_climbing_state));
			return static_cast<gom::Gameplay_state*> (new (pmem) Player_climbing_state(true));
			//return new Player_climbing_state(true);
		}
		/*
		else { // no ladder, change to ducking state
			//animation
			actor.get_anim_controller_component()->set_bool("is_ducking", true);
			//gameplay
			
			void *pmem = mem::allocate(sizeof(Player_ducking_state));
			return static_cast<gom::Gameplay_state*> (new (pmem) Player_ducking_state);

			//return new Player_ducking_state;
		}*/

	}

    bool is_attack_pressed = io::g_input_mgr.get_button_down(SID('attack_01'));
	if (is_attack_pressed) {
		actor.get_anim_controller_component()->set_trigger(is_attacking_param_id);
		//create projectile
		//gom::Game_object_handle handle = gom::g_game_object_mgr.instantiate(knife_obj_id, actor.get_body_2d_component()->get_position());
		if (actor.get_facing_direction()) {
			gom::Game_object_handle handle = gom::g_projectile_mgr.spawn_projectile(knife_obj_id, actor.get_body_2d_component()->get_position(), math::vec2(-1.0f, 0.0f) );
		}
		else {
			gom::Game_object_handle handle = gom::g_projectile_mgr.spawn_projectile(knife_obj_id, actor.get_body_2d_component()->get_position(), math::vec2(1.0f, 0.0f));
		}
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

	if (pbody->get_collider()->get_aabb().p_min.y >= tile_aabb.p_max.y && (pbody->get_collider()->get_aabb().p_max.x >= tile_aabb.p_min.x && pbody->get_collider()->get_aabb().p_min.x <= tile_aabb.p_max.x) ) {
		std::cout << "Floor tile" << std::endl;
	}
	else if (pbody->get_collider()->get_aabb().p_max.y <= tile_aabb.p_min.y && (pbody->get_collider()->get_aabb().p_max.x >= tile_aabb.p_min.x && pbody->get_collider()->get_aabb().p_min.x <= tile_aabb.p_max.x)) {
		std::cout << "Ceiling tile" << std::endl;
	}
	else if (pbody->get_collider()->get_aabb().p_max.x >= tile_aabb.p_min.x && (pbody->get_collider()->get_aabb().p_min.y < tile_aabb.p_max.y && pbody->get_collider()->get_aabb().p_max.y > tile_aabb.p_min.y)) {
		std::cout << "Right wall tile" << std::endl;
	}
	else if (pbody->get_collider()->get_aabb().p_min.x <= tile_aabb.p_max.x && (pbody->get_collider()->get_aabb().p_min.y < tile_aabb.p_max.y && pbody->get_collider()->get_aabb().p_max.y > tile_aabb.p_min.y)) {
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

	if (pbody->get_collider()->get_aabb().p_min.y >= tile_aabb.p_max.y && (pbody->get_collider()->get_aabb().p_max.x >= tile_aabb.p_min.x && pbody->get_collider()->get_aabb().p_min.x <= tile_aabb.p_max.x)) {
		std::cout << "Floor tile" << std::endl;
	}
	else if (pbody->get_collider()->get_aabb().p_max.y <= tile_aabb.p_min.y && (pbody->get_collider()->get_aabb().p_max.x >= tile_aabb.p_min.x && pbody->get_collider()->get_aabb().p_min.x <= tile_aabb.p_max.x)) {
		std::cout << "Ceiling tile" << std::endl;
	}
	else if (pbody->get_collider()->get_aabb().p_max.x >= tile_aabb.p_min.x && (pbody->get_collider()->get_aabb().p_min.y < tile_aabb.p_max.y && pbody->get_collider()->get_aabb().p_max.y > tile_aabb.p_min.y)) {
		std::cout << "Right wall tile" << std::endl;
	}
	else if (pbody->get_collider()->get_aabb().p_min.x <= tile_aabb.p_max.x && (pbody->get_collider()->get_aabb().p_min.y < tile_aabb.p_max.y && pbody->get_collider()->get_aabb().p_max.y > tile_aabb.p_min.y)) {
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