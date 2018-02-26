#ifndef _PLAYER_JUMPING_STATE_HPP
#define _PLAYER_JUMPING_STATE_HPP

#include "Gameplay_state.hpp"
#include "Actor.hpp"
#include "AABB_2d.hpp"

class Player_jumping_state : public Gameplay_state {
public:
	Player_jumping_state(Actor & actor, float y_acceleration = 0.30f, float x_vel = 5.0f);

	Gameplay_state *  handle_input(Actor & actor) override;
    //void update(Actor & actor) override;

	 void begin_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb) override;
	 void end_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb) override;
private:
	float m_y_acceleration;
	float m_x_vel;
//	bool m_moving_left;
//	bool m_moving_right;
};

#endif // !_PLAYER_JUMPING_STATE_HPP
