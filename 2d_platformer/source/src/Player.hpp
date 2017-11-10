#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#define  PLAYER_ATLAS "C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/resources/sprite sheets/player_sheet.xml"

#include "vec2.hpp"
#include "vec3.hpp"
#include "mat4.hpp"
#include "AABB_2d.hpp"
#include "Actor.hpp"

//enum Input { MOVE_LEFT, MOVE_RIGHT, MOVE_DOWN, MOVE_UP, RELEASE_LEFT, RELEASE_RIGHT, RELEASE_DOWN, RELEASE_UP, JUMP, DUCK, ATTACK };

class Player : public Actor {
public:

	Player(const cgm::vec3 & position, const cgm::mat4 orientation, const AABB_2d & aabb, const cgm::vec2 & velocity);
	
	void handle_input();
	void update() override;
	

private:
	int					m_anim_frame;
	float				m_life;
	//std::unique_ptr<tgs::Animation_player>  m_upanim_player;
};


#endif // !_PLAYER_HPP
