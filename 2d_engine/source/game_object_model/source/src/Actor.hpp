#ifndef _ACTOR_HPP
#define _ACTOR_HPP

#include <string>

#include "vec2.hpp"
#include "AABB_2d.hpp"
#include "Game_object.hpp"



class Gameplay_state;
namespace math { struct vec3; class mat4; }
namespace gfx { class Sprite; }

class Actor : public Game_object {
public:
	Actor(const math::vec3 & pos, const math::mat4 & orientation, gfx::Sprite *psprite, Gameplay_state *pstate, const physics_2d::AABB_2d & aabb, const math::vec2 & velocity, bool facing_left = true);
	Actor(gfx::Sprite *psprite, const physics_2d::AABB_2d & aabb, const math::vec2 & velocity, bool facing_left = true);

	virtual ~Actor() 
	{
		delete m_pstate;
	}

	math::vec2 &	 get_velocity()							 { return m_velocity; }
	Gameplay_state*  get_state()                             { return m_pstate;}
	void             set_state(Gameplay_state *pstate)	     { m_pstate = pstate;}
	bool             get_facing_direction()				     { return m_facing_left; }
	void             set_facing_direction(bool facing_left)  { m_facing_left = facing_left; }

	void			begin_tile_collision(const physics_2d::AABB_2d & tile_aabb);
	void			end_tile_collision(const physics_2d::AABB_2d &   tile_aabb);

	std::string get_type() const override { return "Actor"; }
	virtual void update() = 0;
	
protected:
	Gameplay_state						 *m_pstate;
	physics_2d::AABB_2d					  m_aabb;
	math::vec2							  m_velocity;
	bool								  m_facing_left;
};

#endif // !_ACTOR_HPP
