#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#define  PLAYER_ATLAS "C:/Users/mateu/Documents/GitHub/Demos/2d_game/resources/sprite sheets/character.xml"

#include "Actor.hpp"

namespace cgm { struct vec2; struct vec3; class mat4; }
namespace physics_2d { struct AABB_2d; class Body_2d;  }
namespace gfx { class Animator_controller; }

class Player final : public Actor {
public:

	Player(gfx::Animator_controller *pcontroller, physics_2d::Body_2d *pbody, const cgm::vec3 & position, const cgm::mat4 & orientation, const physics_2d::AABB_2d & aabb, const cgm::vec2 & velocity);
	
	void handle_input();
	void update() override;
	

private:
	int					m_anim_frame;
	float				m_life;
	//std::unique_ptr<tgs::Animation_player>  m_upanim_player;
};


#endif // !_PLAYER_HPP
