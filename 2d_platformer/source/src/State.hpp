#ifndef _STATE_HPP
#define _STATE_HPP

#include "Actor.hpp"
#include "Animation_player.hpp"
#include "Animation.hpp"
#include "Button.hpp"
#include "Command.hpp"
#include <utility>
#include <vector>

//------------------------------------------------------------------------
/* State: interface of a state in a state machine, it has a Animation_player data member
 * so it can play the animations for a specific state, a handle_input function, so it can process commands and,
 *  a update() method to implement the behavior of the state. Every game object that has multiple states will have a pointer
 * to a concrete class that implements this interface, so it can transition beetween states and behave accordly to it's current
 * state
 */
//-------------------------------------------------------------------------

class State {
public:
	State(const tgs::Animation & animation) : m_anim_player({animation}) {}
	virtual ~State() {}
	virtual State * handle_input(const std::vector<std::pair<Button, Command *>> & stream, Actor & actor) = 0;
	virtual void update(Actor & actor) = 0;

	tgs::Animation_player & get_anim_player() { return m_anim_player; }
private:
	tgs::Animation_player m_anim_player;
};


#endif // !_STATE_HPP
