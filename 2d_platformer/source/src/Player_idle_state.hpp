#ifndef _PLAYER_IDLE_STATE_HPP
#define _PLAYER_IDLE_STATE_HPP

#include "Gameplay_state.hpp"
#include "Actor.hpp"

#define  BLINK_INTERVAL 2 // in seconds

class Player_idle_state : public Gameplay_state {
public:
	Gameplay_state *  check_transition(Actor & actor) override;
	void update(Actor & actor) override;
//private:
	//float  m_blink_timer = BLINK_INTERVAL;
	//bool   m_blinked = false;
};



#endif // !_PLAYER_IDLE_STATE_HPP
