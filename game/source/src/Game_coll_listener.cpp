#include "Game_coll_listener.hpp"

#include "AABB_2d.hpp"
#include "Body_2d.hpp"
#include "Game_object.hpp"
#include "Actor.hpp"
#include "Projectile.hpp"
#include "Enemy.hpp"
#include "string_id.hpp"

#include <iostream>

Game_coll_listener::Game_coll_listener() 
{
	m_player_id = intern_string("Player_object");
	m_hover_robot_id = intern_string("hover_robot");
	m_player_projectile_id = intern_string("knife_obj");
}

void Game_coll_listener::begin_body_collision(physics_2d::Body_2d * pbody_a, physics_2d::Body_2d *pbody_b) const 
{
	gom::Game_object  *p_game_object_a = static_cast<gom::Game_object*>(pbody_a->get_user_data());
	gom::Game_object  *p_game_object_b = static_cast<gom::Game_object*>(pbody_b->get_user_data());

	std::cout << "-----------------------------------IN BEGIN COLLISION---------------------------------------------------" << std::endl;

	if (p_game_object_a->get_tag() == m_player_projectile_id && p_game_object_b->get_tag() == m_hover_robot_id) {
		gom::Projectile *pprojectile = static_cast<gom::Projectile*>(p_game_object_a);
		gom::Enemy		*penemy = static_cast<gom::Enemy*>(p_game_object_b);
		penemy->projectile_collision(pprojectile);
	}
	else if (p_game_object_b->get_tag() == m_player_projectile_id && p_game_object_a->get_tag() == m_hover_robot_id) {
		gom::Projectile *pprojectile = static_cast<gom::Projectile*>(p_game_object_b);
		gom::Enemy		*penemy = static_cast<gom::Enemy*>(p_game_object_a);
	}

}
void Game_coll_listener::end_body_collision(physics_2d::Body_2d * pbody_a, physics_2d::Body_2d * pbody_b) const  
{  
	gom::Game_object  *p_game_object_a = static_cast<gom::Game_object*>(pbody_a->get_user_data());
	gom::Game_object  *p_game_object_b = static_cast<gom::Game_object*>(pbody_b->get_user_data());

	std::cout << "-----------------------------------IN END COLLISION---------------------------------------------------" << std::endl;
	if ((p_game_object_a->get_tag() == m_player_id && p_game_object_b->get_tag() == m_hover_robot_id) ||
		(p_game_object_b->get_tag() == m_player_id && p_game_object_a->get_tag() == m_hover_robot_id)) {
		std::cout << "--------------------------PLAYER STOPPED COLLIDING WITH HOVER ROBOT!------------------------------------" << std::endl;
	}

	if ((p_game_object_a->get_tag() == m_player_projectile_id && p_game_object_b->get_tag() == m_hover_robot_id) ||
		(p_game_object_b->get_tag() == m_player_projectile_id && p_game_object_a->get_tag() == m_hover_robot_id)) {
		std::cout << "------------------PROJECTILE STOPPED COLLIDING WITH HOVER ROBOT!---------------------------" << std::endl;
	}
}

/*
void Game_coll_listener::begin_tile_collision(physics_2d::Body_2d & b, const physics_2d::AABB_2d & tile_aabb) const 
{
	gom::Game_object *pgame_obj = static_cast<gom::Game_object *>(b.get_user_data());
	/*
	std::string type = pgame_obj->get_type();

	if (type == "Actor") {
		gom::Actor * pactor = static_cast<gom::Actor*>(pgame_obj);
		pactor->begin_tile_collision(tile_aabb);
	}
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
	
}
*/