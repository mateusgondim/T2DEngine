#ifndef _ACTOR_HPP
#define _ACTOR_HPP


#include <utility>
#include "vec2.hpp"
#include "Game_object.hpp"



namespace gom { class Gameplay_state; }
namespace math { struct vec3; class mat4; }
namespace gfx { class Sprite; class Sprite_atlas; }
namespace physics_2d { struct AABB_2d; class Body_2d; struct Body_2d_def; }

namespace gom {
	class Actor : public Game_object {
	public:
		typedef std::pair<uint8_t, gfx::Sprite_atlas*> sprite_info;

		Actor(const game_object_id unique_id, const uint16_t handle_index, sprite_info & s_info, physics_2d::Body_2d *pbody, const gfx::Animator_controller *pcontroller, bool facing_left = true);

		virtual ~Actor();

		math::vec2 &	 get_velocity() { return m_velocity; }
		Gameplay_state*  get_state() { return m_pstate; }
		void             set_state(Gameplay_state *pstate) { m_pstate = pstate; }
		bool             get_facing_direction() { return m_facing_left; }
		void             set_facing_direction(bool facing_left) { m_facing_left = facing_left; }

		void			begin_tile_collision(const physics_2d::AABB_2d & tile_aabb);
		void			end_tile_collision(const physics_2d::AABB_2d &   tile_aabb);

		virtual void update(const float dt) = 0;

	protected:
		Gameplay_state						 *m_pstate;
		math::vec2							  m_velocity;
		bool								  m_facing_left;
	};
}
#endif // !_ACTOR_HPP
