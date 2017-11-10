#ifndef _PLAYER_IDLE_STATE_HPP
#define _PLAYER_IDLE_STATE_HPP

#include "Gameplay_state.hpp"
#include "Actor.hpp"
#include "AABB_2d.hpp"

#define  BLINK_INTERVAL 2 // in seconds

class Player_idle_state : public Gameplay_state {
public:
	Gameplay_state *  check_transition(Actor & actor) override;
	void update(Actor & actor) override;

	void begin_tile_collision(Actor & actor, const AABB_2d & tile_aabb) override;
	void end_tile_collision(Actor & actor, const AABB_2d & tile_aabb) override;
//private:
	//float  m_blink_timer = BLINK_INTERVAL;
	//bool   m_blinked = false;
};



#endif // !_PLAYER_IDLE_STATE_HPP
