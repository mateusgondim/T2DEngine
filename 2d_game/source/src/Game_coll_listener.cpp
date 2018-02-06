#include "Game_coll_listener.hpp"
#include "Collision_listener.hpp"
#include "Body_2d.hpp"
#include "AABB_2d.hpp"
#include "Game_object.hpp"
#include "Actor.hpp"

void Game_coll_listener::begin_tile_collision(physics_2d::Body_2d & b, const AABB_2d & tile_aabb) const 
{
	Game_object *pgame_obj = static_cast<Game_object *>(b.get_user_data());
	std::string type = pgame_obj->get_type();

	if (type == "Actor") {
		Actor * pactor = static_cast<Actor*>(pgame_obj);
		pactor->begin_tile_collision(tile_aabb);
	}
}

void Game_coll_listener::end_tile_collision(physics_2d::Body_2d   & b, const AABB_2d & tile_aabb) const 
{
	Game_object *pgame_obj = static_cast<Game_object*>(b.get_user_data());
	std::string type = pgame_obj->get_type();

	if (type == "Actor") {
		Actor * pactor = static_cast<Actor*>(pgame_obj);
		pactor->end_tile_collision(tile_aabb);
	}
}