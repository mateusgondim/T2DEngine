#ifndef _GAME_COLL_LISTENER_HPP
#define _GAME_COLL_LISTENER_HPP

#include "Collision_listener.hpp"
#include "AABB_2d.hpp"
#include "string_id.hpp"

namespace physics_2d { class Body_2d; struct AABB_2d; }

class Game_coll_listener : public physics_2d::Collision_listener {
public:
	Game_coll_listener();
	void begin_body_collision(physics_2d::Body_2d * pbody_a, physics_2d::Body_2d * pbody_b) const override;
	void end_body_collision(physics_2d::Body_2d * pbody_a, physics_2d::Body_2d * pbody_b) const override;
private:
	string_id     m_player_id;
	string_id     m_hover_robot_id;
	string_id     m_player_projectile_id;
    string_id     m_player_tag;
    string_id     m_enemies_tag;
    string_id     m_player_projectile_tag;
    string_id     m_enemy_projectile_tag;

	//void begin_tile_collision(physics_2d::Body_2d & b, const physics_2d::AABB_2d & tile_aabb) const override;
	//void end_tile_collision(physics_2d::Body_2d   & b, const physics_2d::AABB_2d & tile_aabb) const override;
};

#endif // !_GAME_COLL_LISTENER_HPP
