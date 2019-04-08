#ifndef _PLAYER_DUCKING_STATE_HPP
#define _PLAYER_DUCKING_STATE_HPP

#include "Gameplay_state.hpp"
#include "AABB_2d.hpp"

namespace { class Actor; }
namespace physics_2d { struct AABB_2d; class World;}


class Player_ducking_state : public gom::Gameplay_state {
public:
	gom::Gameplay_state *  handle_input(gom::Actor & actor) override;
	size_t get_size() const override;

	void begin_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb) override;
	void end_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb) override;
};


#endif // !_PLAYER_DUCKING_STATE_HPP
