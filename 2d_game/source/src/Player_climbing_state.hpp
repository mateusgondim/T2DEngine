#ifndef _PLAYER_CLIMBING_STATE_HPP
#define _PLAYER_CLIMBING_STATE_HPP

#include "Gameplay_state.hpp"
#include "Actor.hpp"
#include "AABB_2d.hpp"
#include "vec2.hpp"

namespace physics_2d { class World; }
class Input_manager;

class Player_climbing_state : public gom::Gameplay_state {
public:
	Player_climbing_state(const bool climbing_from_top = false, const math::vec2 & climbing_vel = math::vec2(0.0f, 2.5f));
	Gameplay_state *  handle_input(gom::Actor & actor) override;
	//void update(Actor & actor) override;
	size_t get_size() const override;
	void begin_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb) override;
	void end_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb) override;
private:
	math::vec2  m_climbing_vel;
	bool       m_reached_top = false;
	bool       m_from_top;
	unsigned   m_anim_clip;
};

#endif // !_PLAYER_CLIMBING_STATE_HPP
