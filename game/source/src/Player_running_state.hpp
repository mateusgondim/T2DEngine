#ifndef _PLAYER_RUNNING_STATE_HPP
#define _PLAYER_RUNNING_STATE_HPP
#include "Gameplay_state.hpp"
#include "AABB_2d.hpp"

namespace gom { class Actor; }
namespace physics_2d { struct AABB_2d; }
class Input_manager;

class Player_running_state : public gom::Gameplay_state {
public:
	Player_running_state(gom::Actor & actor, const float x_vel = 3.5f);
	gom::Gameplay_state * handle_input(gom::Actor & actor) override;
	size_t get_size() const override;
	//void update(Actor & actor) override;

	void begin_tile_collision(gom::Actor & actor, const  physics_2d::AABB_2d & tile_aabb)  override;
	void end_tile_collision(gom::Actor & actor, const physics_2d::AABB_2d & tile_aabb)  override;
	void set_targed_x_speed(const float x) { m_x_vel = x; }

private:
	float m_x_vel;
};


#endif // !_PLAYER_RUNNING_STATE_HPP
