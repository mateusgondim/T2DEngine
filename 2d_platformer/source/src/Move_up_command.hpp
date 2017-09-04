#ifndef _MOVE_UP_COMMAND_HPP

#define _MOVE_UP_COMMAND_HPP
#include "command.hpp"
#include "Actor.hpp"


class Move_up_command : public Command {
public:
	virtual void execute(Actor & actor) { actor.move_up();}

};

#endif // !_MOVE_UP_COMMAND_HPP
