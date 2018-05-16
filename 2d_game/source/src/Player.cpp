
#include "Player.hpp"
#include "Actor.hpp"

#include "runtime_memory_allocator.hpp"
#include "vec2.hpp"
#include "vec3.hpp"
#include "mat4.hpp"
#include "Gameplay_state.hpp"
#include "Player_idle_state.hpp"
#include "Animator_state.hpp"
#include "Animator_controller.hpp"
#include "Sprite.hpp"
#include "Body_2d.hpp"


Player::Player(const game_object_id unique_id, const uint16_t handle_index, sprite_info & s_info, physics_2d::Body_2d *pbody, const gfx::Animator_controller *pcontroller, bool facing_left) :
	Actor(unique_id, handle_index, s_info, pbody, pcontroller, facing_left), m_life(100)
{
	void *pmem = mem::allocate(sizeof(Player_idle_state));
	m_pstate = static_cast<gom::Gameplay_state*>(new (pmem) Player_idle_state());
}


void Player::handle_input(Input_manager *pinput, physics_2d::World *pwld)
{
	gom::Gameplay_state * pstate = get_state()->handle_input(*this, pinput, pwld);
	if (pstate) {
		size_t sz = m_pstate->get_size();
		m_pstate->~Gameplay_state();
		mem::free(static_cast<void*>(m_pstate), sz);
		set_state(pstate);
	 }
}

void Player::update(const float dt) 
{
	m_panimator_controller->update(dt);
	
	gfx::Animator_state & curr_state = m_panimator_controller->get_current_state();
	if (curr_state.changed_animation_frame()) {
		m_psprite->update_uv(curr_state.get_curr_anim_frame());
	}
	m_psprite->update_pos(m_pbody_2d->get_position(), !get_facing_direction());
}


