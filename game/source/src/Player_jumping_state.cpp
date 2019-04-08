#include "Player_jumping_state.hpp"
#include "Actor.hpp"
#include "AABB_2d.hpp"

#include <algorithm>

#include "Player.hpp"
#include "Player_taking_hit_state.hpp"
#include "Player_idle_state.hpp"
#include "Player_climbing_state.hpp"

#include "Game_object_handle.hpp"
#include "Projectile_manager.hpp"

#include "Animator_controller.hpp"
#include "Gameplay_state.hpp"
#include "vec2.hpp"

#include "Body_2d.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"
#include "Input_manager.hpp"
#include "Abstract_game_actions_index.hpp"

#include "runtime_memory_allocator.hpp"

Player_jumping_state::Player_jumping_state(gom::Actor & actor, float jump_vel, float x_vel) : Gameplay_state(), m_jump_vel(jump_vel), m_x_vel(x_vel)
{
	actor.get_body_2d_component()->add_to_velocity(math::vec2(0.0f, m_jump_vel));
}

gom::Gameplay_state * Player_jumping_state::handle_input(gom::Actor & actor)
{
	string_id is_attacking_param_id = intern_string("is_attacking");
	string_id player_attacking_state_id = intern_string("player_attacking");
	string_id knife_obj_id = intern_string("knife_obj");
	bool on_ground = physics_2d::g_physics_mgr.get_world()->is_body_2d_on_ground(actor.get_body_2d_component());

    //check if is taking a hit
    Player *pplayer = static_cast<Player*>(&actor);
    if (pplayer->is_taking_hit()) {
            actor.get_anim_controller_component()->set_bool("is_taking_hit", true);
            actor.get_anim_controller_component()->set_bool("is_jumping", false);
            void *pmem = mem::allocate(sizeof(Player_taking_hit_state));
            return static_cast<gom::Gameplay_state*> (new (pmem) Player_taking_hit_state(actor));
    }

	if (on_ground) {
		//std::cout << "changing state to player_idle" << std::endl;
		//set the paramter on the animation state machine to make the transition to the new animation
		actor.get_anim_controller_component()->set_bool("is_jumping", false);
		actor.get_body_2d_component()->stop_movement_x();
		
		void *pmem = mem::allocate(sizeof(Player_idle_state));
		return static_cast<gom::Gameplay_state*> (new (pmem) Player_idle_state);
	}

	if ((actor.get_anim_controller_component()->get_current_state().get_state_id() == player_attacking_state_id)) {
		return nullptr;
	}

    bool is_climb_up_pressed = io::g_input_mgr.get_button_down(Abstract_game_actions_index::CLIMB_UP);
	if (is_climb_up_pressed) {
		bool is_on_ladder = physics_2d::g_physics_mgr.get_world()->try_climbing_ladder(actor.get_body_2d_component(), true);
		if (is_on_ladder) {
			actor.get_anim_controller_component()->set_bool("is_jumping", false);
			actor.get_anim_controller_component()->set_bool("is_climbing", true);
			actor.get_body_2d_component()->stop_movement_x();
			actor.get_body_2d_component()->stop_movement_y();
			actor.get_body_2d_component()->set_gravity_scale(0.0f);
			
			void *pmem = mem::allocate(sizeof(Player_climbing_state));
			return static_cast<gom::Gameplay_state*> (new (pmem) Player_climbing_state);
		}
	}

    bool is_attack_pressed = io::g_input_mgr.get_button_down(Abstract_game_actions_index::ATTACK_01);
	if (is_attack_pressed) {
		//ANIMATION
		actor.get_anim_controller_component()->set_trigger("is_attacking");
		if (actor.get_facing_direction()) {
			gom::Game_object_handle handle = gom::g_projectile_mgr.spawn_projectile(knife_obj_id, actor.get_body_2d_component()->get_position(), math::vec2(-1.0f, 0.0f));
		}
		else {
			gom::Game_object_handle handle = gom::g_projectile_mgr.spawn_projectile(knife_obj_id, actor.get_body_2d_component()->get_position(), math::vec2(1.0f, 0.0f));
		}
		return nullptr;
	}

    bool is_move_left_pressed = io::g_input_mgr.get_button(Abstract_game_actions_index::MOVE_LEFT);
	if (is_move_left_pressed) {
		actor.get_body_2d_component()->stop_movement_x();
		actor.set_facing_direction(true);     //change to running left
		actor.get_body_2d_component()->add_to_velocity(math::vec2(-m_x_vel, 0.0f));
		//std::cout << "-----moving left here---------" << std::endl;
	}
	
    bool is_move_right_pressed = io::g_input_mgr.get_button(Abstract_game_actions_index::MOVE_RIGHT);
	if (is_move_right_pressed) {
		actor.get_body_2d_component()->stop_movement_x();
		actor.set_facing_direction(false);     
		actor.get_body_2d_component()->add_to_velocity(math::vec2(m_x_vel, 0.0f));
		//std::cout << "-----moving left here---------" << std::endl;
	}
	
	if (!is_move_left_pressed && !is_move_right_pressed) {
		actor.get_body_2d_component()->stop_movement_x();
	}
	
	
	return nullptr;
}

size_t Player_jumping_state::get_size() const 
{
	return sizeof(Player_jumping_state);
}

/*void Player_jumping_state::update(Actor & actor) 
{
	actor.get_sprite().update_pos(actor.get_body_2d()->get_position(), actor.get_facing_direction());
}*/

void Player_jumping_state::begin_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << __FUNCTION__ << std::endl;
}

void Player_jumping_state::end_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << __FUNCTION__ << std::endl;
}