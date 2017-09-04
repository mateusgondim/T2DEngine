#ifndef _PLAYER_RUNNING_STATE_HPP
#define _PLAYER_RUNNING_STATE_HPP
#include "Actor.hpp"
#include "State.hpp"
#include "Button.hpp"
#include "Command.hpp"
#include <vector>

#define RUNNING_STATE_FPS 8

class Player_running_state : public State {
public:
	Player_running_state(const std::vector<unsigned> frames, float frames_per_second = RUNNING_STATE_FPS) : State(tgs::Animation(frames, frames_per_second)) {}
	Player_running_state(float frames_per_second = RUNNING_STATE_FPS) : State(tgs::Animation(m_state_anim_frames, frames_per_second)) {}

	virtual State * handle_input(const std::vector<std::pair<Button, Command *>> & stream, Actor & actor);
	virtual void update(Actor & actor);
	static  void switch_anim_frames(const std::vector<unsigned> frames) { m_state_anim_frames = frames; }

private:
	static std::vector<unsigned> m_state_anim_frames;
};


#endif // !_PLAYER_RUNNING_STATE_HPP
