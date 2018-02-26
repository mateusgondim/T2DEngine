#include <memory>
#include <string>

#include "vec3.hpp"
#include "vec2.hpp"
#include "mat4.hpp"
#include "AABB_2d.hpp"

#include "Actor.hpp"
#include "Sprite.hpp"
#include "Gameplay_state.hpp"
#include "Animator_controller.hpp"

//TAKE OUT MAGIC NUMBER 16 ON SPRITE CONSTRUCTOR
//Actor::Actor(const cgm::vec3 & pos, const cgm::mat4 & orientation, const std::string & texture_file, State *pstate, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left)
//	: Renderable_game_object(pos, orientation, texture_file), m_pstate(pstate), m_aabb(aabb), m_velocity(velocity), m_facing_left(facing_left) {}

Actor::Actor(const std::string & texture_file, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left) :  m_aabb(aabb), m_velocity(velocity), m_facing_left(facing_left) 
{
	m_psprite = new tgs::Sprite(texture_file, 16.0f);
}

Actor::Actor(const cgm::vec3 & pos, const cgm::mat4 & orientation, const std::string & texture_file, Gameplay_state *pstate, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left) :
	Game_object(pos), m_pstate(pstate) ,m_aabb(aabb), m_velocity(velocity), m_facing_left(facing_left) 
{
	m_psprite = new tgs::Sprite(texture_file, 16.0f);
}

void Actor::begin_tile_collision(const AABB_2d & tile_aabb)  
{
	m_pstate->begin_tile_collision(*this, tile_aabb);
}
void Actor::end_tile_collision(const AABB_2d & tile_aabb)  
{
	m_pstate->end_tile_collision(*this, tile_aabb);
}