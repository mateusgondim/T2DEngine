
#ifndef _MOVE_DOWN_COMMAND_HPP
#define _MOVE_DOWN_COMMAND_HPP
#include "Command.hpp"
#include "Actor.hpp"

class Move_down_command : public Command {
public:
	virtual void execute(Actor & actor) { actor.move_down(); }
};

#endif // !_MOVE_DOWN_COMMAND_HPP
