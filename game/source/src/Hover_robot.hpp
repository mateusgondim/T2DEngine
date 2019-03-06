#ifndef _HOVER_ROBOT_HPP
#define _HOVER_ROBOT_HPP

#include "Actor.hpp"
#include <stdint.h>

namespace gfx { class Sprite; class Animator_controller; }
namespace physics_2d { class Body_2d; struct Body_2d_def; }

class Hover_robot final : public gom::Actor{
public:
	Hover_robot(const game_object_id unique_id, const uint16_t handle_index, atlas_n_layer & sprite_data, physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller, bool facing_left = true);

    void Hover_robot::actor_collision(gom::Actor *pactor) override;
	void update(const float dt) override;
};

#endif // !_HOVER_ROBOT_HPP
