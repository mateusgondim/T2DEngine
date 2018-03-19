#ifndef _PLAYER_RUNNING_STATE_HPP
#define _PLAYER_RUNNING_STATE_HPP
#include "Gameplay_state.hpp"
#include "AABB_2d.hpp"
class Actor;
namespace physics_2d { struct AABB_2d; }

class Player_running_state : public Gameplay_state {
public:
	Player_running_state(Actor & actor, const float acceleration = 3.5f);
	Gameplay_state * handle_input(Actor & actor) override;
	//void update(Actor & actor) override;

	void begin_tile_collision(Actor & actor, const  physics_2d::AABB_2d & tile_aabb)  override;
	void end_tile_collision(Actor & actor, const physics_2d::AABB_2d & tile_aabb)  override;
	void set_targed_x_speed(const float x) { m_acceleration = x; }

private:
	float m_acceleration;
};


#endif // !_PLAYER_RUNNING_STATE_HPP
