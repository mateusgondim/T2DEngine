#ifndef _ACTOR_HPP
#define _ACTOR_HPP

#include "Renderable_game_object.hpp"
#include "AABB_2d.hpp"
#include "vec2.hpp"
#include "Gameplay_state.hpp"
#include "Animator_controller.hpp"
#include <string>
#include <memory>

class Gameplay_state;
class Animator_controller;
class Actor : public Renderable_game_object {
public:
	Actor(const cgm::vec3 & pos, const cgm::mat4 & orientation, const std::string & texture_file, Gameplay_state *pstate, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left = true);
	//Actor(const cgm::vec3 & pos, const cgm::mat4 & orientation, const std::string & texture_file, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left = true);
	Actor(const std::string & texture_file, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left = true);

	//TODO: SETTERS AND GETERS
	cgm::vec2 &		 get_velocity()						     { return m_velocity; }
	Gameplay_state*  get_state()                             { return m_pstate;}
	void             set_state(Gameplay_state *pstate)	     { m_pstate = pstate;}
	bool             get_facing_direction()				     { return m_facing_left; }
	void             set_facing_direction(bool facing_left)  { m_facing_left = facing_left; }

	void       set_anim_controller(std::unique_ptr<Animator_controller> & uptr_anim_controller);
    Animator_controller *get_panim_controller() { return m_upanim_controller.get(); }

	virtual void update(const float delta_time) = 0;
	virtual void move_up() = 0;
	virtual void move_down() = 0;
	virtual void move_left() = 0;
	virtual void move_right() = 0;
private:
	Gameplay_state						 *m_pstate;
	std::unique_ptr<Animator_controller>  m_upanim_controller;
	AABB_2d								  m_aabb;
	cgm::vec2							  m_velocity;
	bool								  m_facing_left;
};

#endif // !_ACTOR_HPP
