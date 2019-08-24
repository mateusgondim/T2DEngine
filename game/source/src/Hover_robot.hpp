#ifndef _HOVER_ROBOT_HPP
#define _HOVER_ROBOT_HPP

#include "Actor.hpp"
#include <stdint.h>
#include <cstddef>

namespace gfx { class Sprite; class Animator_controller; }
namespace physics_2d { class Body_2d; struct Body_2d_def; }
class Event;

class Hover_robot final : public gom::Actor{
public:
	Hover_robot(std::size_t object_sz, atlas_n_layer & sprite_data,
                physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller,
                bool facing_left = true);

	void update(const float dt) override;
    void on_event(Event & event) override;
};

#endif // !_HOVER_ROBOT_HPP
