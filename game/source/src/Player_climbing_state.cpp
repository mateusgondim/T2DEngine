#include "Player_climbing_state.hpp"

#include "Player.hpp"
#include "Player_taking_hit_state.hpp"
#include "Player_idle_state.hpp"
#include "Player_jumping_state.hpp"

#include "Animator_controller.hpp"
#include "Actor.hpp"
#include "AABB_2d.hpp"
#include "Body_2d.hpp"
#include "Collider_2d.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"
#include "Input_manager.hpp"
#include "Rect.hpp"

#include "crc32.hpp"
#include "runtime_memory_allocator.hpp"

Player_climbing_state::Player_climbing_state(const bool climbing_from_top, const math::vec2 & climbing_vel) :
	Gameplay_state(), m_from_top(climbing_from_top), m_climbing_vel(climbing_vel), m_anim_clip(0) {}

size_t Player_climbing_state::get_size() const
{
	return sizeof(Player_climbing_state);
}

gom::Gameplay_state * Player_climbing_state::handle_input(gom::Actor & actor)
{
	math::Rect bounds;
	
	bool is_on_ladder = physics_2d::g_physics_mgr.get_world()->is_body_on_ladder(actor.get_body_2d_component());
	bool is_on_ladder_top = physics_2d::g_physics_mgr.get_world()->is_on_ladder_top_tile(actor.get_body_2d_component(), bounds);
	bool is_on_ground = physics_2d::g_physics_mgr.get_world()->is_body_2d_on_ground(actor.get_body_2d_component());

    //check if is taking a hit
    Player *pplayer = static_cast<Player*>(&actor);
    if (pplayer->is_taking_hit()) {
            actor.get_anim_controller_component()->set_bool("is_taking_hit", true);
            actor.get_anim_controller_component()->set_bool("is_climbing", false);
            void *pmem = mem::allocate(sizeof(Player_taking_hit_state));
            return static_cast<gom::Gameplay_state*> (new (pmem) Player_taking_hit_state(actor));
    }

	//climbing from top animation and small displacement
	if (m_from_top) {
		if (m_anim_clip == 0) {
			actor.get_body_2d_component()->set_velocity(-m_climbing_vel);
			actor.get_anim_controller_component()->set_bool("finish_climbing", true);
			m_anim_clip = 1;
		}
		if (bounds.y > actor.get_body_2d_component()->get_collider()->get_aabb().p_max.y) {
			actor.get_body_2d_component()->stop_movement_y();
			//actor.get_anim_controller_component()->switch_curr_state_anim_clip(0);
			actor.get_anim_controller_component()->set_bool("finish_climbing", false);
			m_from_top = false;
		}
		return nullptr;
	}
	else {
		if (!m_reached_top) { //in the last frame it was still climbing the ladder 
			
			if (!is_on_ladder) { // have fallen of
				actor.get_body_2d_component()->stop_movement_y();
				actor.get_body_2d_component()->set_gravity_scale(1.0f);

				actor.set_facing_direction(true);
				actor.get_anim_controller_component()->set_bool("is_climbing", false);
				actor.get_anim_controller_component()->set_bool("is_jumping", true);
				
				void *pmem = mem::allocate(sizeof(Player_jumping_state));
				return static_cast<gom::Gameplay_state*> (new (pmem) Player_jumping_state(actor, 0.0f));
				//return new Player_jumping_state(actor, 0.0f);
			}

            bool move_left_pressed = io::g_input_mgr.get_button_down(SID('move_left'));
			if (move_left_pressed) {
				actor.get_body_2d_component()->stop_movement_y();
				actor.get_body_2d_component()->set_gravity_scale(1.0f);

				actor.set_facing_direction(true);
				actor.get_anim_controller_component()->set_bool("is_climbing", false);
				actor.get_anim_controller_component()->set_bool("is_jumping", true);
				
				void *pmem = mem::allocate(sizeof(Player_jumping_state));
				return static_cast<gom::Gameplay_state*> (new (pmem) Player_jumping_state(actor, 0.0f));
				//return new Player_jumping_state(actor, 0.0f);
			}

            bool move_right_pressed = io::g_input_mgr.get_button_down(SID('move_right'));
			if (move_right_pressed) {
				actor.get_body_2d_component()->stop_movement_y();
				actor.get_body_2d_component()->set_gravity_scale(1.0f);

				actor.set_facing_direction(false);

				actor.get_anim_controller_component()->set_bool("is_climbing", false);
				actor.get_anim_controller_component()->set_bool("is_jumping", true);
				
				void *pmem = mem::allocate(sizeof(Player_jumping_state));
				return static_cast<gom::Gameplay_state*> (new (pmem) Player_jumping_state(actor, 0.0f));
				//return new Player_jumping_state(actor, 0.0f);
			}


            bool move_up_pressed = io::g_input_mgr.get_button(SID('move_up'));
			if (move_up_pressed) {
				actor.get_body_2d_component()->set_velocity(m_climbing_vel);
				actor.get_anim_controller_component()->get_current_state().resume_anim();
			}
			else {
                    bool move_down_pressed = io::g_input_mgr.get_button(SID('move_down'));
				if (is_on_ground) {
					actor.get_body_2d_component()->stop_movement_y();
					actor.get_body_2d_component()->set_gravity_scale(1.0f);
					actor.get_anim_controller_component()->set_bool("is_climbing", false);
					
					void *pmem = mem::allocate(sizeof(Player_idle_state));
					return static_cast<gom::Gameplay_state*> (new (pmem) Player_idle_state);
					//return new Player_idle_state();
				}

				if (move_down_pressed) {
					actor.get_body_2d_component()->set_velocity(-m_climbing_vel);
					actor.get_anim_controller_component()->get_current_state().resume_anim();
				}
				else if (!move_down_pressed && !move_up_pressed) {
					actor.get_body_2d_component()->stop_movement_y();
					actor.get_anim_controller_component()->get_current_state().pause_anim();
				}
			}

			if (is_on_ladder_top) {//REACHED THE TOP
				//std::cout << "-----------------reached top of ladder---------- " << std::endl;
				if (actor.get_body_2d_component()->get_velocity().y > 0.0f && (bounds.y <= actor.get_body_2d_component()->get_collider()->get_aabb().p_max.y)) {
					m_reached_top = true;
					actor.get_body_2d_component()->set_velocity(math::vec2(0.0f, m_climbing_vel.y));
					//start climbing off animation
					actor.get_anim_controller_component()->set_bool("finish_climbing", true);
					//actor.get_anim_controller_component()->switch_curr_state_anim_clip(1);
					return nullptr;
				}
				else {
					return nullptr;
				}
			}
			return nullptr;
		}
		else { //climbing off from the top, one tile
			float center_y = (actor.get_body_2d_component()->get_collider()->get_aabb().p_max.y + actor.get_body_2d_component()->get_collider()->get_aabb().p_min.y) / 2.0f;
			if (center_y > bounds.y) {
				float y_displacement = bounds.y - actor.get_body_2d_component()->get_collider()->get_aabb().p_min.y;
				math::vec2  translation(0.0f, y_displacement);
				actor.get_body_2d_component()->translate_by(translation);

				actor.get_body_2d_component()->stop_movement_y();
				actor.get_body_2d_component()->set_gravity_scale(1.0f);
				actor.get_anim_controller_component()->set_bool("is_climbing", false);
				actor.get_anim_controller_component()->set_bool("finish_climbing", false);
				
				//actor.get_anim_controller_component()->switch_curr_state_anim_clip(0);
				
				void *pmem = mem::allocate(sizeof(Player_idle_state));
				return static_cast<gom::Gameplay_state*> (new (pmem) Player_idle_state);

				//return new Player_idle_state();
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


void Player_climbing_state::begin_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << __FUNCTION__ << std::endl;
}

void Player_climbing_state::end_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)
{
	std::cout << __FUNCTION__ << std::endl;
}