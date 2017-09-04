#ifndef _PLAYER_IDLE_STATE_HPP
#define _PLAYER_IDLE_STATE_HPP

#include "State.hpp"
#define  IDLE_STATE_FPS 5
#define  BLINK_INTERVAL 2 // in seconds

class Player_idle_state : public State {
public:
	Player_idle_state(const std::vector<unsigned> frames, float frames_per_second = IDLE_STATE_FPS) : State(tgs::Animation(frames, frames_per_second)) {}
	Player_idle_state(float frames_per_second = IDLE_STATE_FPS) : State(tgs::Animation(m_state_anim_frames, frames_per_second ) ) {}
	virtual State * handle_input(const std::vector<std::pair<Button, Command *>> & stream, Actor & actor);
	virtual void update(Actor & actor);
	static  void switch_anim_frames(const std::vector<unsigned> frames) { m_state_anim_frames = frames; }
private:
	static std::vector<unsigned> m_state_anim_frames;
	float  m_blink_timer = BLINK_INTERVAL;
	bool   m_blinked = false;
};



#endif // !_PLAYER_IDLE_STATE_HPP
