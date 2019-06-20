#include "Player_ducking_state.hpp"

#include "AABB_2d.hpp"
#include "Actor.hpp"
#include "Gameplay_state.hpp"
#include "Input_manager.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"
#include "Animator_controller.hpp"

#include "Projectile_manager.hpp"
#include "Game_object_handle.hpp"

#include "Player.hpp"
#include "Player_taking_hit_state.hpp"
#include "Player_idle_state.hpp"

#include "crc32.hpp"
#include "runtime_memory_allocator.hpp"

gom::Gameplay_state * Player_ducking_state::handle_input(gom::Actor & actor)
{
	string_id is_attacking_param_id = intern_string("is_attacking");
	string_id player_attacking_state_id = intern_string("player_ducking_attacking");
	string_id knife_obj_id = intern_string("knife_obj");
	//check if is attacking, if so, return

    //check if is taking a hit
    Player *pplayer = static_cast<Player*>(&actor);
    if (pplayer->is_taking_hit()) {
            actor.get_anim_controller_component()->set_bool("is_ducking", false);
            actor.get_anim_controller_component()->set_bool("is_taking_hit", true);
            void *pmem = mem::allocate(sizeof(Player_taking_hit_state));
            return static_cast<gom::Gameplay_state*> (new (pmem) Player_taking_hit_state(actor));
    }
	
	if ((actor.get_anim_controller_component()->get_current_state().get_state_id() == player_attacking_state_id)) {
		return nullptr;
	}

	//check if released ducking button, if so, return to idle
    bool is_move_down_released = io::g_input_mgr.get_button_up(SID('move_down'));
	if (is_move_down_released) {
		//ANIMATION
		actor.get_anim_controller_component()->set_bool("is_ducking", false);
		//Gameplay
		void *pmem = mem::allocate(sizeof(Player_idle_state));
		return static_cast<gom::Gameplay_state*> (new (pmem) Player_idle_state);
		
		//return new Player_idle_state;
	}

	//check if pressed left or right, if so, change orientation
    bool is_move_left_pressed = io::g_input_mgr.get_button_down(SID('move_left'));
	if (is_move_left_pressed) {
		//change orientation
		actor.set_facing_direction(true);
		return nullptr;
	}
	else { 
        bool is_move_right_pressed = io::g_input_mgr.get_button_down(SID('move_right'));
		if (is_move_right_pressed) {
			actor.set_facing_direction(false);
			return nullptr;
		}
	}
	
	//check if pressed attack button, if so, attack
    bool is_attack_pressed = io::g_input_mgr.get_button_down(SID('attack_01'));

	if (is_attack_pressed) {
		//ANIMATION
		actor.get_anim_controller_component()->set_trigger("is_attacking");
		//gameplay
		if (actor.get_facing_direction()) {
			gom::Game_object_handle handle = gom::g_projectile_mgr.spawn_projectile(knife_obj_id, actor.get_body_2d_component()->get_position(), math::vec2(-1.0f, 0.0f));
		}
		else {
			gom::Game_object_handle handle = gom::g_projectile_mgr.spawn_projectile(knife_obj_id, actor.get_body_2d_component()->get_position(), math::vec2(1.0f, 0.0f));
		}
	}
	return nullptr;
}

size_t Player_ducking_state::get_size() const 
{
	return sizeof(Player_ducking_state);
}

void Player_ducking_state::begin_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	;
}

void Player_ducking_state::end_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	;
}