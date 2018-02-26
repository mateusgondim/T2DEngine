
#include "Player.hpp"

#include "vec2.hpp"
#include "vec3.hpp"
#include "mat4.hpp"
#include "AABB_2d.hpp"
#include "Gameplay_state.hpp"
#include "Player_idle_state.hpp"
#include "Animator_controller.hpp"
#include "Engine.hpp"

Player::Player(gfx::Animator_controller *pcontroller, physics_2d::Body_2d *pbody,const cgm::vec3 & position, const cgm::mat4 & orientation, const physics_2d::AABB_2d & aabb, const cgm::vec2 & velocity) :
	Actor(position, orientation, PLAYER_ATLAS, new Player_idle_state ,aabb, velocity, false), m_anim_frame(0), m_life(100)
{
	//std::vector<unsigned> running_frames = { 3, 4, 5 }; // running
	//std::vector<unsigned> idle_frames = { 0, 1 };
	//INITIALIZE THE STATE FRAMES VECTOR IN A INITIALIZATION CODE INSIDE game.cpp, later on engine.cpp
	m_panimator_controller = pcontroller;
	m_pbody_2d             = pbody;
	m_psprite->update_pos(position);
}


void Player::handle_input() 
{
	Gameplay_state * pstate = get_state()->handle_input(*this);
	if (pstate) {
		delete get_state();
		set_state(pstate);
	 }
}

void Player::update() 
{
	m_panimator_controller->update(g_engine.m_timer.get_dt());
	m_psprite->update_animation(g_engine.m_timer.get_dt(), m_panimator_controller);
	
	m_psprite->update_pos(m_pbody_2d->get_position(), !get_facing_direction());
}


