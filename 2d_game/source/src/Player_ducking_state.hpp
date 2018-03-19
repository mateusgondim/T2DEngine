#ifndef _PLAYER_DUCKING_STATE_HPP
#define _PLAYER_DUCKING_STATE_HPP

#include "Gameplay_state.hpp"
#include "AABB_2d.hpp"

class Actor;
namespace physics_2d { struct AABB_2d; }

class Player_ducking_state : public Gameplay_state {
public:
	Gameplay_state *  handle_input(Actor & actor) override;

	void begin_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb) override;
	void end_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb) override;
};


#endif // !_PLAYER_DUCKING_STATE_HPP
