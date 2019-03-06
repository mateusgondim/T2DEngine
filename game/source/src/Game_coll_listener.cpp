#include "Game_coll_listener.hpp"

#include "AABB_2d.hpp"
#include "Body_2d.hpp"
#include "Game_object.hpp"
#include "Actor.hpp"
#include "Projectile.hpp"
#include "Enemy.hpp"
#include "Player.hpp"
#include "string_id.hpp"

#include <iostream>

Game_coll_listener::Game_coll_listener() 
{
	m_player_id = intern_string("Player_object");
	m_hover_robot_id = intern_string("hover_robot");
	m_player_projectile_id = intern_string("knife_obj");

    m_enemies_tag = intern_string("Enemy");
    m_player_tag = intern_string("Player");
    m_player_projectile_tag = intern_string("p_projectile");
    m_enemy_projectile_tag = intern_string("e_projectile");
}

void Game_coll_listener::begin_body_collision(physics_2d::Body_2d * pbody_a, physics_2d::Body_2d *pbody_b) const 
{
	gom::Game_object  *p_game_object_a = static_cast<gom::Game_object*>(pbody_a->get_user_data());
	gom::Game_object  *p_game_object_b = static_cast<gom::Game_object*>(pbody_b->get_user_data());

	std::cout << "-----------------------------------IN BEGIN COLLISION---------------------------------------------------" << std::endl;

    if (p_game_object_a->get_tag() == m_player_tag && p_game_object_b->get_tag() == m_enemies_tag) {
            gom::Actor *pplayer = static_cast<gom::Actor*>(p_game_object_a);
            gom::Actor *penemy  = static_cast<gom::Actor*>(p_game_object_b);
            pplayer->actor_collision(penemy);
            penemy->actor_collision(pplayer);
            return;
    }
    else if (p_game_object_b->get_tag() == m_player_tag && p_game_object_a->get_tag() == m_enemies_tag) {
            gom::Actor *pplayer = static_cast<gom::Actor*>(p_game_object_b);
            gom::Actor *penemy = static_cast<gom::Actor*>(p_game_object_a);
            pplayer->actor_collision(penemy);
            penemy->actor_collision(pplayer);
            return;
    }

    if (p_game_object_a->get_tag() == m_player_projectile_tag && p_game_object_b->get_tag() == m_enemies_tag) {
            gom::Projectile *pprojectile = static_cast<gom::Projectile*>(p_game_object_a);
            gom::Actor		*penemy = static_cast<gom::Actor*>(p_game_object_b);
            penemy->projectile_collision(pprojectile);
            return;
    }
    else if (p_game_object_b->get_tag() == m_player_projectile_tag && p_game_object_a->get_tag() == m_enemies_tag) {
            gom::Projectile *pprojectile = static_cast<gom::Projectile*>(p_game_object_b);
            gom::Actor      *penemy = static_cast<gom::Actor*>(p_game_object_a);
            penemy->projectile_collision(pprojectile);
            return;
    }

	// if (p_game_object_a->get_type() == m_player_projectile_id && p_game_object_b->get_type() == m_hover_robot_id) {
	// 	gom::Projectile *pprojectile = static_cast<gom::Projectile*>(p_game_object_a);
	// 	gom::Actor		*penemy = static_cast<gom::Actor*>(p_game_object_b);
	// 	penemy->projectile_collision(pprojectile);
	// }
	// else if (p_game_object_b->get_type() == m_player_projectile_id && p_game_object_a->get_type() == m_hover_robot_id) {
	// 	gom::Projectile *pprojectile = static_cast<gom::Projectile*>(p_game_object_b);
	// 	gom::Actor      *penemy = static_cast<gom::Actor*>(p_game_object_a);
	// }
    
}
void Game_coll_listener::end_body_collision(physics_2d::Body_2d * pbody_a, physics_2d::Body_2d * pbody_b) const  
{  
	gom::Game_object  *p_game_object_a = static_cast<gom::Game_object*>(pbody_a->get_user_data());
	gom::Game_object  *p_game_object_b = static_cast<gom::Game_object*>(pbody_b->get_user_data());

	std::cout << "-----------------------------------IN END COLLISION---------------------------------------------------" << std::endl;
	if ((p_game_object_a->get_tag() == m_player_tag && p_game_object_b->get_tag() == m_enemies_tag) ||
		(p_game_object_b->get_tag() == m_player_tag && p_game_object_a->get_tag() == m_enemies_tag)) {
		std::cout << "--------------------------PLAYER STOPPED COLLIDING WITH HOVER ROBOT!------------------------------------" << std::endl;
        return;
	}

	if ((p_game_object_a->get_tag() == m_player_projectile_tag && p_game_object_b->get_tag() == m_enemies_tag) ||
		(p_game_object_b->get_tag() == m_player_projectile_tag && p_game_object_a->get_tag() == m_enemies_tag)) {
		std::cout << "------------------PROJECTILE STOPPED COLLIDING WITH HOVER ROBOT!---------------------------" << std::endl;
        return;
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