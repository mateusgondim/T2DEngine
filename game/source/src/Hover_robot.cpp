#include "Hover_robot.hpp"

#include "Game_object_handle.hpp"
#include "Actor.hpp"
#include "Animator_controller.hpp"
#include "Animator_state.hpp"
#include "Sprite.hpp"

#include "vec3.hpp"
#include "ray3.hpp"
#include "AABB_2d.hpp"
#include "crc32.hpp"
#include "Variant.hpp"
#include "Event_arguments.hpp"
#include "Event.hpp"
#include "Collider_2d.hpp"
#include "Body_2d.hpp"
#include "Tile.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"
#include "Game_object_manager.hpp"

#include <cstdint>

Hover_robot::Hover_robot(std::size_t object_sz, atlas_n_layer & sprite_data,
                         physics_2d::Body_2d_def *pbody_def,
                         const gfx::Animator_controller *pcontroller, bool facing_left) : 
                           Actor(object_sz, sprite_data, pbody_def, pcontroller, facing_left) 
{
        m_health = 100;
        m_damage = 25;
        m_velocity.x = -1.0f;
        m_velocity.y = 0.0f;
        get_body_2d_component()->add_to_velocity(m_velocity);

}

void Hover_robot::on_event(Event & event)
{
        switch (event.get_type()) {
        case SID('EVENT_IN_COLLISION'):
        case SID('EVENT_BEGIN_COLLISION'): {
                // Get colliding Game Object
                const Variant *phandle_arg = event.get_arguments()
                                                  .find(SID('game_object_handle_index'));

                const Variant *pobject_id_arg = event.get_arguments().find(SID('game_object_id'));
                gom::Game_object_handle handle(pobject_id_arg->m_as_string_id,
                                               phandle_arg->m_as_uint16);

                Game_object * pgame_object = gom::g_game_object_mgr.get_by_handle(handle);
                if (!pgame_object) {
                        std::cout << __FUNCTION__ << " ERROR: Could not find Game object"
                                << std::endl;
                        break;
                }

                // Create attack Event
                Event attack_event(SID('EVENT_ATTACK'));
                attack_event.get_arguments().insert(SID('attack_points'), get_attack_points());

                // send to game object
                pgame_object->on_event(attack_event);
                break;
        }
        case SID('EVENT_END_COLLISION'):
                break;
        case SID('EVENT_PROJECTILE_ATTACK'): {
                // Get colliding Projectile
                const Variant *phandle_arg = event.get_arguments()
                                                  .find(SID('game_object_handle_index'));

                const Variant *pobject_id_arg = event.get_arguments().find(SID('game_object_id'));
                gom::Game_object_handle handle(pobject_id_arg->m_as_string_id,
                                               phandle_arg->m_as_uint16);

                Game_object * pgame_object = gom::g_game_object_mgr.get_by_handle(handle);
                if (!pgame_object) {
                        std::cout << __FUNCTION__ << " ERROR: Could not find Game object"
                                << std::endl;
                        break;
                }
                const Variant * pattack_points_arg = event.get_arguments()
                        .find(SID('attack_points'));
                m_health -= pattack_points_arg->m_as_integer;

                // send hit target event
                Event hit_target_event(SID('EVENT_HIT_TARGET'));
                pgame_object->on_event(hit_target_event);
                break;
        }
        default:
                std::cout << __FUNCTION__ << ": Receiving UNRECOGNIZABLE Event" << std::endl;
                break;
        }
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
