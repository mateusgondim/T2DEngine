#include "Hover_robot.hpp"
#include "Enemy.hpp"

#include "Animator_controller.hpp"
#include "Animator_state.hpp"
#include "Sprite.hpp"

#include "Body_2d.hpp"

Hover_robot::Hover_robot(const game_object_id unique_id, const uint16_t handle_index, atlas_n_layer & sprite_data, physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller, bool facing_left)
	: Enemy(unique_id, handle_index, sprite_data, pbody_def, pcontroller, facing_left, 100, 25) {}

void Hover_robot::update(const float dt) 
{
	if (m_health <= 0) {
		set_active(false);
	}

	m_panimator_controller->update(dt);

	gfx::Animator_state & curr_state = m_panimator_controller->get_current_state();
	if (curr_state.changed_animation_frame()) {
		m_psprite->update_uv(curr_state.get_curr_anim_frame());
	}
	m_psprite->update_pos(m_pbody_2d->get_position(), !get_facing_direction());
}
