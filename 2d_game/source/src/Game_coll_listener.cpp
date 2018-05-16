#include "Game_coll_listener.hpp"

#include "AABB_2d.hpp"
#include "Body_2d.hpp"
#include "Game_object.hpp"
#include "Actor.hpp"

#include <string>

void Game_coll_listener::begin_body_collision(physics_2d::Body_2d & b1, physics_2d::Body_2d & b2) const { ; }
void Game_coll_listener::end_body_collision(physics_2d::Body_2d & b1, physics_2d::Body_2d & b2) const  { ; }

void Game_coll_listener::begin_tile_collision(physics_2d::Body_2d & b, const physics_2d::AABB_2d & tile_aabb) const 
{
	gom::Game_object *pgame_obj = static_cast<gom::Game_object *>(b.get_user_data());
	/*
	std::string type = pgame_obj->get_type();

	if (type == "Actor") {
		gom::Actor * pactor = static_cast<gom::Actor*>(pgame_obj);
		pactor->begin_tile_collision(tile_aabb);
	}*/
}

void Game_coll_listener::end_tile_collision(physics_2d::Body_2d   & b, const physics_2d::AABB_2d & tile_aabb) const
{
	gom::Game_object *pgame_obj = static_cast<gom::Game_object*>(b.get_user_data());
	/*
	std::string type = pgame_obj->get_type();

	if (type == "Actor") {
		gom::Actor * pactor = static_cast<gom::Actor*>(pgame_obj);
		pactor->end_tile_collision(tile_aabb);
	}
	*/
}
