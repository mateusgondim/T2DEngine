#ifndef _GAMEPLAY_STATE_HPP
#define _GAMEPLAY_STATE_HPP
#include <cstddef>
//------------------------------------------------------------------------
/* Gameplay_State: Abstract Base Class, derive this class to create a state of a gameplay state machine for some Actor in the Game. 
 * It has a update() method to implement the behavior of the actor in this gameplay state. Every game object that has multiple gameplay states will have a pointer
 * to a concrete class that implements this interface, so it can transition beetween states and behave accordly to it's current state
 */
//-------------------------------------------------------------------------

namespace physics_2d { struct AABB_2d; class Body_2d;}

namespace gom { class Actor; }

namespace gom {
	class Gameplay_state {
	public:
		Gameplay_state() = default;
		virtual ~Gameplay_state() {}

		virtual Gameplay_state *  handle_input(Actor & actor) = 0;
		virtual size_t			  get_size() const = 0;
		//virtual void update(Actor & actor) = 0;

		//virtual void begin_body_collision(physics_2d::Body_2d & body) const = 0;
		//virtual void end_body_collision(physics_2d::Body_2d & body) const = 0;
		virtual void begin_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb) = 0;
		virtual void end_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb) = 0;
	};
}

#endif // !_GAMEPLAY_STATE_HPP
