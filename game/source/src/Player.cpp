
#include "Player.hpp"
#include "Actor.hpp"

#include "runtime_memory_allocator.hpp"
#include "vec2.hpp"
#include "vec3.hpp"
#include "mat4.hpp"
#include "crc32.hpp"
#include "Variant.hpp"
#include "Event_arguments.hpp"
#include "Event.hpp"
#include "Gameplay_state.hpp"
#include "Player_idle_state.hpp"
#include "Animator_state.hpp"
#include "Animator_controller.hpp"
#include "Sprite.hpp"
#include "Body_2d.hpp"
#include "Level_manager.hpp"

#include <iostream>


Player::Player(const game_object_id unique_id, const uint16_t handle_index, atlas_n_layer & sprite_data, physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller, bool facing_left) :
	Actor(unique_id, handle_index, sprite_data, pbody_def, pcontroller, facing_left)
{
	void *pmem = mem::allocate(sizeof(Player_idle_state));
	m_pstate = static_cast<gom::Gameplay_state*>(new (pmem) Player_idle_state());
    m_health = 100;
    m_taking_hit = false;
}


void Player::handle_input()
{
	gom::Gameplay_state * pstate = get_state()->handle_input(*this);
	if (pstate) {
		size_t sz = m_pstate->get_size();
		m_pstate->~Gameplay_state();
		mem::free(static_cast<void*>(m_pstate), sz);
		set_state(pstate);
	 }
}

void Player::on_event(Event & event)
{
        switch (event.get_type()) {
        case SID('EVENT_BEGIN_COLLISION'):
        //        std::cout << __FUNCTION__ << ": Receiving EVENT_BEGIN_COLLISION" << std::endl;
                break;
        case SID('EVENT_END_COLLISION') :
                // std::cout << __FUNCTION__ << ": Receiving EVENT_END_COLLISION" << std::endl;
                break;
        case SID('EVENT_ATTACK'): {
                // std::cout << __FUNCTION__ << ": Receiving EVENT_ATTACK" << std::endl;
                const Variant * pattack_arg = event.get_arguments().find(SID('attack_points'));
                m_health -= pattack_arg->m_as_integer;
                m_taking_hit = true;
                break;
        }
        default:
                // std::cout << __FUNCTION__ << ": Receiving UNRECOGNIZABLE Event" << std::endl;
                break;

        }
}

// REMOVE THIS FUNCTION
void Player::actor_collision(gom::Actor *pactor)
{
        m_health -= pactor->get_attack_points();
        m_taking_hit = true;
}

void Player::update(const float dt) 
{
        if (!gom::g_level_mgr.is_game_clock_paused()) {
                handle_input();
        }
	m_panimator_controller->update(dt);
	gfx::Animator_state & curr_state = m_panimator_controller->get_current_state();
	if (curr_state.changed_animation_frame()) {
		m_psprite->update_uv(curr_state.get_curr_anim_frame());
	}
	m_psprite->update_pos(m_pbody_2d->get_position(), !get_facing_direction());
}


