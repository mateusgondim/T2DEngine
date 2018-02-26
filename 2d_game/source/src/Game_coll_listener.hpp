#ifndef _GAME_COLL_LISTENER_HPP
#define _GAME_COLL_LISTENER_HPP

#include "Collision_listener.hpp"
#include "AABB_2d.hpp"


namespace physics_2d { class Body_2d; }

class Game_coll_listener : public physics_2d::Collision_listener {
public:
	void begin_body_collision(physics_2d::Body_2d & b1, physics_2d::Body_2d & b2) const override;
	void end_body_collision(physics_2d::Body_2d & b1, physics_2d::Body_2d & b2) const override;

	void begin_tile_collision(physics_2d::Body_2d & b, const physics_2d::AABB_2d & tile_aabb) const override;
	void end_tile_collision(physics_2d::Body_2d   & b, const physics_2d::AABB_2d & tile_aabb) const override;
};

#endif // !_GAME_COLL_LISTENER_HPP
