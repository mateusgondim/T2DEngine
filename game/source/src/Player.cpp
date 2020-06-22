
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


unsigned Player::s_NUM_SECONDS_TO_RESPAWN = 2;
unsigned Player::s_NUM_SECONDS_IN_INVINCIBILIY_MODE = 1;

Player::Player(std::size_t object_sz, atlas_n_layer & sprite_data,
               physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller,
               bool facing_left) :
	             Actor(object_sz, sprite_data, pbody_def, pcontroller, facing_left)
{
	void *pmem = mem::allocate(sizeof(Player_idle_state));
	m_pstate = static_cast<gom::Gameplay_state*>(new (pmem) Player_idle_state());
    m_health = 100;
    m_taking_hit = false;
    m_is_player_dead = false;
    m_utility_timer = 0.0f;
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
    if (m_is_player_dead || m_taking_hit) {
        return;
    }
    switch (event.get_type()) {
    case SID('EVENT_BEGIN_COLLISION'):
        break;
    case SID('EVENT_END_COLLISION'):
        break;
    case SID('EVENT_IN_COLLISION'):
        break;
    case SID('EVENT_ATTACK'): {
        if (m_is_in_invincibiliy_mode) {
            break;
        }
        const Variant * pattack_arg = event.get_arguments().find(SID('attack_points'));
        m_health -= pattack_arg->m_as_integer;
        m_taking_hit = true;
        if (m_health <= 0) {
            std::cout << "PLAYER IS DEAD!" << std::endl;
            m_is_player_dead = true;
            m_utility_timer = 0.0f;
        }
        break;
    }
    default:
        break;

    }
}

void Player::start_invincibiliy_mode()
{
    if (!m_is_in_invincibiliy_mode) {
        m_is_in_invincibiliy_mode = true;
        m_utility_timer = 0.0f;
    }
}

void Player::update(const float dt) 
{
    if (m_is_player_dead) {
        m_utility_timer += dt;
        if (m_utility_timer >= s_NUM_SECONDS_TO_RESPAWN) {
            level_management::g_level_mgr.request_restart();
        }
    }
    else if (m_is_in_invincibiliy_mode) {
        m_utility_timer += dt;
        if (m_utility_timer >= s_NUM_SECONDS_IN_INVINCIBILIY_MODE) {
            m_is_in_invincibiliy_mode = false;
            m_utility_timer = 0.0f;
        }
    }


    if (!level_management::g_level_mgr.is_game_clock_paused()) {
        handle_input();
    }

	m_panimator_controller->update(dt);
	gfx::Animator_state & curr_state = m_panimator_controller->get_current_state();
	if (curr_state.changed_animation_frame()) {
		m_psprite->update_uv(curr_state.get_curr_anim_frame());
	}
	m_psprite->update_pos(m_pbody_2d->get_position(), !get_facing_direction());
}


