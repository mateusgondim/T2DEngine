#ifndef _COMMAND_HPP
#define _COMMAND_HPP

#include "Actor.hpp"
/*Command: implementation of the command pattern
 *
 */

class Command {
public:
	virtual void execute(Actor & actor) = 0;

};

#endif // !_COMMAND_HPP
