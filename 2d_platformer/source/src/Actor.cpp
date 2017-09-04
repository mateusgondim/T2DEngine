#include "Actor.hpp"

Actor::Actor(const cgm::vec3 & pos, const cgm::mat4 & orientation, const std::string & texture_file, State *pstate, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left)
	: Renderable_game_object(pos, orientation, texture_file), m_pstate(pstate), m_aabb(aabb), m_velocity(velocity), m_facing_left(facing_left) {}

Actor::Actor(const std::string & texture_file, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left) :
	Renderable_game_object(texture_file), m_aabb(aabb), m_velocity(velocity), m_facing_left(facing_left) {}