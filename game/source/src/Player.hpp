#ifndef _PLAYER_HPP
#define _PLAYER_HPP
#include "Actor.hpp"
#include <cstddef>

namespace math { struct vec2; struct vec3; class mat4; }
namespace physics_2d { struct AABB_2d; class Body_2d; class World; }
namespace gfx { class Sprite; class Animator_controller; }
class Event;

class Player final : public gom::Actor {
public:
        Player(std::size_t object_sz, atlas_n_layer & sprite_data,
               physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller,
               bool facing_left = true);

        void            handle_input();
        void            update(const float dt) override;
        bool            is_taking_hit() const { return m_taking_hit; }
        void            set_taking_hit(const bool taking_hit) { m_taking_hit = taking_hit; }
        void            on_event(Event & event) override;
        bool            is_player_dead() const { return m_is_player_dead; }
private:
        bool            m_taking_hit;
        bool            m_is_player_dead;

        // used to count time elapsed after important events
        float           m_utility_timer;
        static  unsigned s_NUM_SECONDS_TO_RESPAWN;
};


#endif // !_PLAYER_HPP
