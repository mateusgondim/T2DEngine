#ifndef _ACTOR_HPP
#define _ACTOR_HPP

#include "Renderable_game_object.hpp"
#include "AABB_2d.hpp"
#include "vec2.hpp"
#include <string>

class State;
class Actor : public Renderable_game_object {
public:
	Actor(const cgm::vec3 & pos, const cgm::mat4 & orientation, const std::string & texture_file, State *pstate, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left = true);
	Actor(const std::string & texture_file, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left = true);

	//TODO: SETTERS AND GETERS
	cgm::vec2 & get_velocity()						   { return m_velocity; }
	State*      get_state()                            { return m_pstate;}
	void        set_state(State * pstate)			   { m_pstate = pstate;}
	bool        get_facing_direction()				   { return m_facing_left; }
	void        set_facing_direction(bool facing_left) { m_facing_left = facing_left; }

	virtual void update(const float delta_time) = 0;
	virtual void move_up() = 0;
	virtual void move_down() = 0;
	virtual void move_left() = 0;
	virtual void move_right() = 0;
private:
	State							*m_pstate;
	AABB_2d					        m_aabb;
	cgm::vec2						m_velocity;
	bool							m_facing_left;
};

#endif // !_ACTOR_HPP
