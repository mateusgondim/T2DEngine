#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#define  PLAYER_ATLAS "C:/Users/mateu/Documents/GitHub/Demos/2d_game/resources/sprite sheets/character.xml"
#include "Collision_listener.hpp"
#include "Animator_controller.hpp"
#include "Body_2d.hpp"
#include "AABB_2d.hpp"
#include "Actor.hpp"

//enum Input { MOVE_LEFT, MOVE_RIGHT, MOVE_DOWN, MOVE_UP, RELEASE_LEFT, RELEASE_RIGHT, RELEASE_DOWN, RELEASE_UP, JUMP, DUCK, ATTACK };

class Player final : public Actor {
public:

	Player(Animator_controller *pcontroller, physics_2d::Body_2d *pbody, const cgm::vec3 & position, const cgm::mat4 orientation, const AABB_2d & aabb, const cgm::vec2 & velocity);
	
	void handle_input();
	void update() override;
	

private:
	int					m_anim_frame;
	float				m_life;
	//std::unique_ptr<tgs::Animation_player>  m_upanim_player;
};


#endif // !_PLAYER_HPP
