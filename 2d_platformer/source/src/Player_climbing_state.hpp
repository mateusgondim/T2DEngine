#ifndef _PLAYER_CLIMBING_STATE_HPP
#define _PLAYER_CLIMBING_STATE_HPP

#include "Gameplay_state.hpp"
#include "Actor.hpp"
#include "AABB_2d.hpp"
#include "vec2.hpp"

class Player_climbing_state : public Gameplay_state {
public:
	Player_climbing_state(const bool climbing_from_top = false, const cgm::vec2 & climbing_vel = cgm::vec2(0.0f, 0.05f));
	Gameplay_state *  check_transition(Actor & actor) override;
	void update(Actor & actor) override;

	void begin_tile_collision(Actor & actor, const AABB_2d & tile_aabb) override;
	void end_tile_collision(Actor & actor, const AABB_2d & tile_aabb) override;
private:
	cgm::vec2  m_climbing_vel;
	bool       m_reached_top = false;
	bool       m_from_top;
	unsigned   m_anim_clip;
};

#endif // !_PLAYER_CLIMBING_STATE_HPP
