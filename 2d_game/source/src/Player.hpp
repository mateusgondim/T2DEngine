#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#define  PLAYER_ATLAS "C:/Users/mateu/Documents/GitHub/Demos/2d_game/resources/sprite sheets/character.xml"

#include "Actor.hpp"

namespace math { struct vec2; struct vec3; class mat4; }
namespace physics_2d { struct AABB_2d; class Body_2d; class World; }
namespace gfx { class Sprite; class Animator_controller; }
class Input_manager;

class Player final : public gom::Actor {
public:
	Player(const game_object_id unique_id, const uint16_t handle_index, sprite_info & s_info, physics_2d::Body_2d *pbody, const gfx::Animator_controller *pcontroller, bool facing_left = true);
	
	void handle_input(Input_manager *pinput, physics_2d::World *pwld);
	void update(const float dt) override;
private:
	float				m_life;

};


#endif // !_PLAYER_HPP
