#include "Player_ducking_state.hpp"

#include "AABB_2d.hpp"
#include "Actor.hpp"
#include "Gameplay_state.hpp"
#include "input_manager.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"
#include "Animator_controller.hpp"

#include "Player_idle_state.hpp"

#include "runtime_memory_allocator.hpp"

gom::Gameplay_state * Player_ducking_state::handle_input(gom::Actor & actor)
{
	string_id is_attacking_param_id = intern_string("is_attacking");
	string_id player_attacking_state_id = intern_string("player_ducking_attacking");

	//check if is attacking, if so, return
	
	if ((actor.get_anim_controller_component()->get_current_state().get_state_id() == player_attacking_state_id)) {
		return nullptr;
	}

	//check if released ducking button, if so, return to idle
	const Button & move_down_button = io::get_button_from_action(io::GAME_ACTIONS::MOVE_DOWN);
	if (move_down_button.m_state == RELEASED) {
		//ANIMATION
		actor.get_anim_controller_component()->set_bool("is_ducking", false);
		//Gameplay
		void *pmem = mem::allocate(sizeof(Player_idle_state));
		return static_cast<gom::Gameplay_state*> (new (pmem) Player_idle_state);
		
		//return new Player_idle_state;
	}

	//check if pressed left or right, if so, change orientation
	const Button & move_left_button   = io::get_button_from_action(io::GAME_ACTIONS::MOVE_LEFT);
	if (move_left_button.m_state == PRESSED) {
		//change orientation
		actor.set_facing_direction(true);
		return nullptr;
	}
	else { 
		const Button & move_right_button = io::get_button_from_action(io::GAME_ACTIONS::MOVE_RIGHT);
		if (move_right_button.m_state == PRESSED) {
			actor.set_facing_direction(false);
			return nullptr;
		}
	}
	
	//check if pressed attack button, if so, attack
	const Button & attacking_button = io::get_button_from_action(io::GAME_ACTIONS::ATTACK_01);

	if (attacking_button.m_state == PRESSED) {
		//ANIMATION
		actor.get_anim_controller_component()->set_trigger("is_attacking");
		//gameplay
		//throw projectile...
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