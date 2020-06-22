#include "Engine_collision_listener.hpp"
#include "Body_2d.hpp"
#include "Event_arguments.hpp"
#include "Event.hpp"
#include "Game_object.hpp"
#include "crc32.hpp"

Engine_collision_listener::Engine_collision_listener()
{
       m_begin_collision_id = get_crc32("EVENT_BEGIN_COLLISION");
       m_in_collision_id = get_crc32("EVENT_IN_COLLISION");
       m_end_collision_id = get_crc32("EVENT_END_COLLISION");
       m_game_object_id = get_crc32("game_object_id");
       m_game_object_handle_index = get_crc32("game_object_handle_index");
}

void Engine_collision_listener::send_collision_event(gom::Game_object * pfrom_object, 
                                                     gom::Game_object * pto_object,
                                                     std::uint32_t event_type) const
{
        Event coll_event(event_type);
        Event_arguments & arguments = coll_event.get_arguments();
        arguments.insert(m_game_object_id, pfrom_object->get_unique_id());
        arguments.insert(m_game_object_handle_index, pfrom_object->get_handle_index());

        pto_object->on_event(coll_event);
}

void Engine_collision_listener::begin_body_collision(physics_2d::Body_2d * pbody_a,
                                                     physics_2d::Body_2d * pbody_b) const
{
        gom::Game_object  *pgame_object_a = static_cast<gom::Game_object*>(pbody_a->get_user_data());
        gom::Game_object  *pgame_object_b = static_cast<gom::Game_object*>(pbody_b->get_user_data());

        send_collision_event(pgame_object_a, pgame_object_b, m_begin_collision_id);
        send_collision_event(pgame_object_b, pgame_object_a, m_begin_collision_id);
}

void Engine_collision_listener::in_body_collision(physics_2d::Body_2d * pbody_a,
                                                  physics_2d::Body_2d * pbody_b) const
{
        gom::Game_object  *pgame_object_a = static_cast<gom::Game_object*>(pbody_a->get_user_data());
        gom::Game_object  *pgame_object_b = static_cast<gom::Game_object*>(pbody_b->get_user_data());

        send_collision_event(pgame_object_a, pgame_object_b, m_in_collision_id);
        send_collision_event(pgame_object_b, pgame_object_a, m_in_collision_id);
}

void Engine_collision_listener::end_body_collision(physics_2d::Body_2d * pbody_a,
                                                   physics_2d::Body_2d * pbody_b) const
{
        gom::Game_object  *pgame_object_a = static_cast<gom::Game_object*>(pbody_a->get_user_data());
        gom::Game_object  *pgame_object_b = static_cast<gom::Game_object*>(pbody_b->get_user_data());

        send_collision_event(pgame_object_a, pgame_object_b, m_end_collision_id);
        send_collision_event(pgame_object_b, pgame_object_a, m_end_collision_id);
}