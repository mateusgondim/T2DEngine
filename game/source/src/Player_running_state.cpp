
#include "Player_running_state.hpp"

#include "Actor.hpp"
#include "AABB_2d.hpp"
#include "Physics_manager.hpp"
#include "Gameplay_state.hpp"

#include "Game_object_handle.hpp"
#include "Projectile_manager.hpp"

#include "Player.hpp"
#include "Player_taking_hit_state.hpp"
#include "Player_idle_state.hpp"
#include "Player_jumping_state.hpp"
#include "Animation_player.hpp"
#include "Animator_controller.hpp"
#include "Timer.hpp"
#include "Body_2d.hpp"
#include "World.hpp"
#include "Input_manager.hpp"
#include "crc32.hpp"

#include "runtime_memory_allocator.hpp"

#include <iostream>
#include <algorithm>


Player_running_state::Player_running_state(gom::Actor & actor, const float x_vel) : Gameplay_state(), m_x_vel(x_vel)
{
	if (actor.get_facing_direction()) {
		actor.get_body_2d_component()->add_to_velocity(math::vec2(-m_x_vel, 0.0f));
	}
	else {
		actor.get_body_2d_component()->add_to_velocity(math::vec2(m_x_vel, 0.0f));
	}
}

gom::Gameplay_state * Player_running_state::handle_input(gom::Actor & actor)
{
	//auto stream = Input_handler::instance().get_input();
	bool on_ground = physics_2d::g_physics_mgr.get_world()->is_body_2d_on_ground(actor.get_body_2d_component());
	string_id knife_obj_id = intern_string("knife_obj");

    //check if is taking a hit
    Player *pplayer = static_cast<Player*>(&actor);
    if (pplayer->is_taking_hit()) {
            actor.get_anim_controller_component()->set_bool("is_taking_hit", true);
            actor.get_anim_controller_component()->set_bool("is_running", false);
            void *pmem = mem::allocate(sizeof(Player_taking_hit_state));
            return static_cast<gom::Gameplay_state*> (new (pmem) Player_taking_hit_state(actor));
    }

	if (!on_ground) { //player fell
		actor.get_anim_controller_component()->set_bool("is_running", false);
		actor.get_anim_controller_component()->set_bool("is_jumping", true);
		//actor.get_body_2d()->stop_movement_x();


		void *pmem = mem::allocate(sizeof(Player_jumping_state));
		return static_cast<gom::Gameplay_state*> (new (pmem) Player_jumping_state(actor, 0.0f) );
	}
	else {
		if (actor.get_facing_direction()) {
            bool is_move_left_released = io::g_input_mgr.get_button_up(SID('move_left'));
			if (is_move_left_released) {
				//std::cout << "chaging state to Player_idle" << std::endl;
				//set the paramter on the animation state machine to make the transition to the new animation
				actor.get_anim_controller_component()->set_bool("is_running", false);
				actor.get_body_2d_component()->stop_movement_x();
				
				void *pmem = mem::allocate(sizeof(Player_idle_state));
				return static_cast<gom::Gameplay_state*> (new (pmem) Player_idle_state);
			}
		}
		else {
            bool is_move_right_released = io::g_input_mgr.get_button_up(SID('move_right'));
			if (is_move_right_released) {
				//std::cout << "changing state to player_idle" << std::endl;
				//set the paramter on the animation state machine to make the transition to the new animation
				actor.get_anim_controller_component()->set_bool("is_running", false);
				actor.get_body_2d_component()->stop_movement_x();
				
				void *pmem = mem::allocate(sizeof(Player_idle_state));
				return static_cast<gom::Gameplay_state*> (new (pmem) Player_idle_state);
			}
		}

        bool is_jump_pressed = io::g_input_mgr.get_button(SID('jump'));
		if (is_jump_pressed && on_ground) {
			//std::cout << "Changing  to player_jumping_state " << std::endl;
			//set the paramter on the animation state machine to make the transition to the new animation
			actor.get_anim_controller_component()->set_bool("is_running", false);
			actor.get_anim_controller_component()->set_bool("is_jumping", true);
			//actor.get_body_2d()->stop_movement_x();
			
			void *pmem = mem::allocate(sizeof(Player_jumping_state));
			return static_cast<gom::Gameplay_state*> (new (pmem) Player_jumping_state(actor));
		}
        bool is_attack_pressed = io::g_input_mgr.get_button_down(SID('attack_01'));
		if (is_attack_pressed) {
			//ANIMATION
			actor.get_anim_controller_component()->set_bool("is_running", false);
			actor.get_anim_controller_component()->set_trigger("is_attacking");
			
			//GAMEPLAY
			actor.get_body_2d_component()->stop_movement_x();
			//launch projectile
			if (actor.get_facing_direction()) {
				gom::Game_object_handle handle = gom::g_projectile_mgr.spawn_projectile(knife_obj_id, actor.get_body_2d_component()->get_position(), math::vec2(-1.0f, 0.0f));
			}
			else {
				gom::Game_object_handle handle = gom::g_projectile_mgr.spawn_projectile(knife_obj_id, actor.get_body_2d_component()->get_position(), math::vec2(1.0f, 0.0f));
			}

			void *pmem = mem::allocate(sizeof(Player_idle_state));
			return static_cast<gom::Gameplay_state*> (new (pmem) Player_idle_state);
		}
	}
	return nullptr;
}

size_t Player_running_state::get_size() const 
{
	return sizeof(Player_running_state);
}

void Player_running_state::begin_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << "PLAYER RUNNING STATE " << __FUNCTION__ << ": with ";

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
void Player_running_state::end_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << "PLAYER RUNNING STATE " << __FUNCTION__ << ": with ";

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
void Player_running_state::update(Actor & actor) 
{
	actor.get_sprite().update_pos(actor.get_body_2d()->get_position(), actor.get_facing_direction());
}*/