#ifndef _PLAYER_IDLE_STATE_HPP
#define _PLAYER_IDLE_STATE_HPP

#include "Gameplay_state.hpp"
#include "AABB_2d.hpp"

namespace gom { class Actor; }
namespace physics_2d { struct AABB_2d; class World; }

class Player_idle_state : public gom::Gameplay_state {
public:
	gom::Gameplay_state *  handle_input(gom::Actor & actor) override;
	size_t get_size() const override;
	//void update(Actor & actor) override;

	virtual void begin_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb) override;
	virtual void end_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb) override;
//private:
	//float  m_blink_timer = BLINK_INTERVAL;
	//bool   m_blinked = false;
};



#endif // !_PLAYER_IDLE_STATE_HPP
