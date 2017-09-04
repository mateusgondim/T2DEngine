#include "Player.hpp"
#include "State.hpp"
#include "Player_idle_state.hpp"
#include "Player_running_state.hpp"
#include "Timer.hpp"
#include "Animation.hpp"
#include "Animation_player.hpp"
#include "vec2.hpp"
#include "Input_handler.hpp"
#include "Button.hpp"
#include "Command.hpp"
#include <iostream>
#include <vector>
#include <algorithm>




Player::Player(const cgm::vec3 & position, const cgm::mat4 orientation, const AABB_2d & aabb, const cgm::vec2 & velocity) :
	Actor(position, orientation, PLAYER_ATLAS, new Player_idle_state ,aabb, velocity, false), m_anim_frame(0), m_life(100)
{
	//std::vector<unsigned> running_frames = { 3, 4, 5 }; // running
	//std::vector<unsigned> idle_frames = { 0, 1 };
	//INITIALIZE THE STATE FRAMES VECTOR IN A INITIALIZATION CODE INSIDE game.cpp, later on engine.cpp
	get_sprite().update_pos(position);
}


void Player::handle_input() 
{
	const std::vector<std::pair<Button, Command *>> & stream = Input_handler::instance().get_input();

	State * pstate = get_state() ->handle_input(stream, *this);
	if (pstate) {
		delete get_state();
		set_state(pstate);
		get_sprite().update_uv(pstate->get_anim_player().get_frame());//change to the first frame of the new animation
	}
}

void Player::update(const float delta_time) 
{
	get_state()->update(*this);
}

void Player::move_left() 
{
	std::cout << "Player: Moving left" << std::endl;
}

void Player::move_right() 
{
	std::cout << "Player: Moving right" << std::endl;
}
void Player::move_down() 
{
	std::cout << "Player: Moving down" << std::endl;

}
void Player::move_up() 
{
	std::cout << "Player: Moving up" << std::endl;

}
/*
//SWITCH ON STATE FIRST!
void Player::update(const float delta_time) {
	get_velocity() = cgm::vec2(3.0f, 3.0f);

	const std::vector<std::pair<Button, Command *>> & stream = Input_handler::instance().get_input();
	
	auto iter = std::find_if(stream.begin(), stream.end(), 
		[](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == KEY_A; });
	if (iter != stream.end()) {
		if ( (iter->first).m_state == PRESSED) {
			iter->second->execute(*this);
		}
	}

	iter = std::find_if(stream.begin(), stream.end(),
		[](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == KEY_D; });
	if (iter != stream.end()) {
		if ((iter->first).m_state == PRESSED) {
			iter->second->execute(*this);
		}
	}

	iter = std::find_if(stream.begin(), stream.end(),
		[](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == KEY_W; });
	if (iter != stream.end()) {
		if ((iter->first).m_state == PRESSED) {
			iter->second->execute(*this);
		}
	}

	iter = std::find_if(stream.begin(), stream.end(),
		[](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == KEY_S; });
	if (iter != stream.end()) {
		if ((iter->first).m_state == PRESSED) {
			iter->second->execute(*this);
		}
	}

	m_upanim_player->update();
	if (m_anim_frame != m_upanim_player->get_frame()) {
		m_anim_frame = m_upanim_player->get_frame();
		///change texture coordinates
		//PUT THIS ON A DIFFERENT FUCTION, I.E, NOT PLAYER'S UPDATE FUNCTION, IT IS A RENDERING FUCTNIO!!!!!
		get_sprite().update_uv(m_anim_frame);
	}

	get_sprite().update_pos(get_position(), m_facing_left);
	//change the position buffer

}*/

