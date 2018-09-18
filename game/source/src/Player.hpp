#ifndef _PLAYER_HPP
#define _PLAYER_HPP
#include "Actor.hpp"

namespace math { struct vec2; struct vec3; class mat4; }
namespace physics_2d { struct AABB_2d; class Body_2d; class World; }
namespace gfx { class Sprite; class Animator_controller; }

class Player final : public gom::Actor {
public:
        Player(const game_object_id unique_id, const uint16_t handle_index, atlas_n_layer & sprite_data, physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller, bool facing_left = true);

        void            handle_input();
        void            update(const float dt) override;
private:
        float           m_life;

};


#endif // !_PLAYER_HPP
