#ifndef _MOVE_RIGHT_COMMAND_HPP

#define _MOVE_RIGHT_COMMAND_HPP

#include "Command.hpp"
#include "Actor.hpp"

class Move_right_command : public Command {
public:
	virtual void execute(Actor & actor) { actor.move_right();}
};

#endif // !_MOVE_RIGHT_COMMAND_HPP
