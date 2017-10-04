#include <memory>
#include <string>

#include "vec3.hpp"
#include "vec2.hpp"
#include "mat4.hpp"
#include "AABB_2d.hpp"

#include "Actor.hpp"
#include "Renderable_game_object.hpp"
#include "Gameplay_state.hpp"
#include "Animator_controller.hpp"

//TAKE OUT MAGIC NUMBER 16 ON SPRITE CONSTRUCTOR
//Actor::Actor(const cgm::vec3 & pos, const cgm::mat4 & orientation, const std::string & texture_file, State *pstate, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left)
//	: Renderable_game_object(pos, orientation, texture_file), m_pstate(pstate), m_aabb(aabb), m_velocity(velocity), m_facing_left(facing_left) {}

Actor::Actor(const AABB_2d & aabb, const cgm::vec2 & velocity, const std::string & texture_file, const float pixels_per_unit, bool facing_left) :
	m_sprite(texture_file, pixels_per_unit), m_aabb(aabb), m_velocity(velocity), m_facing_left(facing_left) {}

Actor::Actor(const cgm::vec3 & pos, const cgm::mat4 & orientation, Gameplay_state *pstate, const AABB_2d & aabb, const cgm::vec2 & velocity, const std::string & texture_file, const float pixels_per_unit, bool facing_left)
	: Game_object(pos, orientation), m_sprite(texture_file, pixels_per_unit), m_pstate(pstate) ,m_aabb(aabb), m_velocity(velocity), m_facing_left(facing_left) {}

tgs::Sprite &	Actor::get_sprite() { return m_sprite; }