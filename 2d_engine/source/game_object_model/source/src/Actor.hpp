#ifndef _ACTOR_HPP
#define _ACTOR_HPP

#include <string>
#include <memory>

#include "vec2.hpp"
#include "vec3.hpp"
#include "mat4.hpp"
#include "AABB_2d.hpp"
#include "Game_object.hpp"
#include "Gameplay_state.hpp"



class Gameplay_state;

class Actor : public Game_object {
public:
	Actor(const cgm::vec3 & pos, const cgm::mat4 & orientation, const std::string & texture_file, Gameplay_state *pstate, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left = true);
	Actor(const std::string & texture_file, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left = true);

	virtual ~Actor() 
	{
		delete m_pstate;
	}

	cgm::vec2 &		 get_velocity()						     { return m_velocity; }
	Gameplay_state*  get_state()                             { return m_pstate;}
	void             set_state(Gameplay_state *pstate)	     { m_pstate = pstate;}
	bool             get_facing_direction()				     { return m_facing_left; }
	void             set_facing_direction(bool facing_left)  { m_facing_left = facing_left; }

	void			begin_tile_collision(const AABB_2d & tile_aabb);
	void			end_tile_collision(const AABB_2d &   tile_aabb);

	std::string get_type() const override { return "Actor"; }
	virtual void update() = 0;
	
protected:
	Gameplay_state						 *m_pstate;
	AABB_2d								  m_aabb;
	cgm::vec2							  m_velocity;
	bool								  m_facing_left;
};

#endif // !_ACTOR_HPP
