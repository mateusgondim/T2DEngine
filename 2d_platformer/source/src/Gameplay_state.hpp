#ifndef _GAMEPLAY_STATE_HPP
#define _GAMEPLAY_STATE_HPP

#include "Actor.hpp"
#include "Body_2d.hpp"
#include "AABB_2d.hpp"


//------------------------------------------------------------------------
/* Gameplay_State: Abstract Base Class, derive this class to create a state of a gameplay state machine for some Actor in the Game. 
 * It has a update() method to implement the behavior of the actor in this gameplay state. Every game object that has multiple gameplay states will have a pointer
 * to a concrete class that implements this interface, so it can transition beetween states and behave accordly to it's current state
 */
//-------------------------------------------------------------------------

class Actor;
class Gameplay_state {
public:
	Gameplay_state() = default;
	virtual ~Gameplay_state() {}
	
	virtual Gameplay_state *  check_transition(Actor & actor) = 0;
	virtual void update(Actor & actor) = 0;
	
	//virtual void begin_body_collision(physics_2d::Body_2d & body) const = 0;
	//virtual void end_body_collision(physics_2d::Body_2d & body) const = 0;
	virtual void begin_tile_collision(Actor & actor, const AABB_2d & tile_aabb) = 0;
	virtual void end_tile_collision(Actor & actor, const AABB_2d & tile_aabb) = 0;
};


#endif // !_GAMEPLAY_STATE_HPP
