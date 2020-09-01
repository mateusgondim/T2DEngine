#include "Damage_region.hpp"

#include <iostream>

#include "Collider_2d_def.hpp"
#include "Collider_2d.hpp"
#include "Body_2d_def.hpp"
#include "Body_2d.hpp"
#include "Physics_manager.hpp"
#include "World.hpp"

#include "Game_object_handle.hpp"
#include "Game_object_manager.hpp"

#include "Event.hpp"
#include "crc32.hpp"

Damage_region::Damage_region(const math::Rect & bounds, const int damage)
    : gom::Game_object(sizeof(Damage_region), bounds.center()),
      m_bounds(bounds),
      m_damage_points(damage)
{
    physics_2d::Body_2d_def body_def;
    body_def.m_position = bounds.center();
    body_def.m_aabb.p_min = bounds.min();
    body_def.m_aabb.p_max = bounds.max();

    body_def.m_map_collision = false;

    m_pbody_2d = physics_2d::g_physics_mgr.get_world()->create_body_2d(body_def);
    m_pbody_2d->set_user_data(static_cast<void*>(this));

    physics_2d::Collider_2d_def collider_def;
    collider_def.m_aabb = body_def.m_aabb;
    collider_def.m_is_trigger = true;

    m_pbody_2d->create_collider_2d(collider_def);
}

void Damage_region::update(const float dt) {}

void Damage_region::on_event(Event & event)
{
    switch (event.get_type()) {
    case SID('EVENT_IN_COLLISION'):
    case SID('EVENT_BEGIN_COLLISION'): {
        // Get colliding Game Object
        const Variant *phandle_arg = event.get_arguments().find(SID('game_object_handle_index'));

        const Variant *pobject_id_arg = event.get_arguments().find(SID('game_object_id'));
        gom::Game_object_handle handle(pobject_id_arg->m_as_string_id, phandle_arg->m_as_uint16);

        gom::Game_object * pgame_object = gom::g_game_object_mgr.get_by_handle(handle);
        if (!pgame_object) {
            std::cout << __FUNCTION__ << " ERROR: Could not find Game object"
                << std::endl;
            break;
        }

        // Create attack Event
        Event attack_event(SID('EVENT_ATTACK'));
        attack_event.get_arguments().insert(SID('attack_points'), m_damage_points);

        // send to game object
        pgame_object->on_event(attack_event);
        break;
    }
    default:
        break;
    }
}