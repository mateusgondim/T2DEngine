#ifndef _ENGINE_COLLISION_LISTENER_HPP
#define _ENGINE_COLLISION_LISTENER_HPP
#include "Collision_listener.hpp"
#include <cstdint>

namespace physics_2d { class Body_2d; }
namespace gom { class Game_object; }

class Engine_collision_listener : public physics_2d::Collision_listener {
public:
        Engine_collision_listener();
        void begin_body_collision(physics_2d::Body_2d * pbody_a, physics_2d::Body_2d * pbody_b) const override;
        void end_body_collision(physics_2d::Body_2d * pbody_a, physics_2d::Body_2d * pbody_b) const override;
private:
        void  send_collision_event(gom::Game_object * pfrom_object, 
                                   gom::Game_object * pto_object,
                                   std::uint32_t event_type) const;

        std::uint32_t           m_begin_collision_id;
        std::uint32_t           m_end_collision_id;
        std::uint32_t           m_game_object_handle_index;
};
#endif // !_ENGINE_COLLISION_LISTENER_HPP
