#ifndef _MOVE_LEFT_COMMAND_HPP
#define _MOVE_LEFT_COMMAND_HPP

#include "Command.hpp"
#include "Actor.hpp"

class Move_left_command : public Command {
public:
	virtual void execute(Actor & actor) { actor.move_left();}
};

#endif // !_MOVE_LEFT_COMMAND_HPP

