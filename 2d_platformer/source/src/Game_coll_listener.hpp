#ifndef _GAME_COLL_LISTENER_HPP
#define _GAME_COLL_LISTENER_HPP

#include "Collision_listener.hpp"
#include "Body_2d.hpp"
#include "AABB_2d.hpp"

class Game_coll_listener : public physics_2d::Collision_listener {
public:
	void begin_body_collision(physics_2d::Body_2d & b1, physics_2d::Body_2d & b2) const override { return; }
	void end_body_collision(physics_2d::Body_2d & b1, physics_2d::Body_2d & b2) const override { return; }

	void begin_tile_collision(physics_2d::Body_2d & b, const AABB_2d & tile_aabb) const override;
	void end_tile_collision(physics_2d::Body_2d   & b, const AABB_2d & tile_aabb) const override;
};

#endif // !_GAME_COLL_LISTENER_HPP
