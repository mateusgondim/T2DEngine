#ifndef _PLAYER_RUNNING_STATE_HPP
#define _PLAYER_RUNNING_STATE_HPP
#include "Actor.hpp"
#include "Gameplay_state.hpp"
#include "Button.hpp"
#include "Command.hpp"
#include <vector>


class Player_running_state : public Gameplay_state {
public:
    Gameplay_state * check_transition(Actor & actor) override;
	void update(Actor & actor) override;
};


#endif // !_PLAYER_RUNNING_STATE_HPP
