#include "Hover_robot.hpp"
#include "Enemy.hpp"

#include "Animator_controller.hpp"
#include "Animator_state.hpp"
#include "Sprite.hpp"

#include "vec3.hpp"
#include "ray3.hpp"
#include "AABB_2d.hpp"
#include "Collider_2d.hpp"
#include "Body_2d.hpp"
#include "Tile.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"

Hover_robot::Hover_robot(const game_object_id unique_id, const uint16_t handle_index, atlas_n_layer & sprite_data, physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller, bool facing_left)
	: Enemy(unique_id, handle_index, sprite_data, pbody_def, pcontroller, facing_left, 100, 25) 
{
        m_velocity.x = -1.0f;
        m_velocity.y = 0.0f;
        get_body_2d_component()->add_to_velocity(m_velocity);

}

void Hover_robot::update(const float dt) 
{
	if (m_health <= 0) {
		set_active(false);
	}

    //if the velocity on x axis is zero, turn the other way
    if (get_body_2d_component()->get_velocity().x == 0.0f) { // collided with wall
            (m_facing_left) ? (get_body_2d_component()->add_to_velocity(-m_velocity)) : (get_body_2d_component()->add_to_velocity(m_velocity));
            m_facing_left = !m_facing_left;
    }
    else { // check ground
            physics_2d::AABB_2d aabb = get_body_2d_component()->get_collider()->get_aabb();
            math::ray3 r;
            const Tile **pptile = nullptr;

            const math::vec2 & vel = get_body_2d_component()->get_velocity();
            r.origin = (vel.x > 0.0f) ? (math::vec3(aabb.p_max.x, aabb.p_min.y)) : (math::vec3(aabb.p_min.x, aabb.p_min.y));
            r.direction = math::vec3(0.0F, -1.0F, 0.0F);
            bool is_not_near_ledge = physics_2d::g_physics_mgr.get_world()->trace_on_map(r, pptile, 1);
            if (!is_not_near_ledge) {
                    (m_facing_left) ? (get_body_2d_component()->add_to_velocity(-2.0f * m_velocity)) : (get_body_2d_component()->add_to_velocity(2.0f * m_velocity));
                    m_facing_left = !m_facing_left;
            }
    }

	m_panimator_controller->update(dt);

	gfx::Animator_state & curr_state = m_panimator_controller->get_current_state();
	if (curr_state.changed_animation_frame()) {
		m_psprite->update_uv(curr_state.get_curr_anim_frame());
	}
	m_psprite->update_pos(m_pbody_2d->get_position(), !get_facing_direction());
}
