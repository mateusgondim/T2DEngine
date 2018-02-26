#ifndef _BODY_2D_HPP
#define _BODY_2D_HPP
#include "vec2.hpp"
#include "AABB_2d.hpp"

/* Class that defines a physical entity that is affected by
 *  the physics engine
 */

//TODO: Mayby add a gravity scale...
// A (0, 0) vel_threshold means object can accelerate to infinity

namespace physics_2d {
	class Body_2d final {
	friend class World;
	public:
		enum Entity_types { KINEMATIC, DYNAMIC };

		Entity_types		get_type()		const { return m_type;}
		cgm::vec2			get_position()      const { return m_position;}
		cgm::vec2        &  get_position()			  { return m_position; }
		cgm::vec2			get_velocity()      const { return m_velocity;}
		cgm::vec2			get_acceleration()  const { return m_acceleration;}
		AABB_2d				get_aabb_2d()       const { return m_aabb; }
		AABB_2d          &  get_aabb_2d()			 { return m_aabb; }

		void               *get_user_data()     const { return m_user_data; }
		void                set_user_data(void * pdata) { m_user_data = pdata; }
		

		void                set_velocity_threshold(const cgm::vec2 & vel_threshold) { m_vel_threshold = vel_threshold; }
		void                add_force(const cgm::vec2 f) { m_velocity.x += f.x; m_velocity.y += f.y;} // remember to add vector operations on cgm ....
		void                stop_movement_x() { m_velocity.x = m_acceleration.x = 0.0f; }
		void                stop_movement_y() { m_velocity.y = m_acceleration.y = 0.0f; }
		void                set_velocity(const cgm::vec2 vel) { m_velocity = vel; }
		void                apply_gravity(const bool gravity_on) { m_apply_gravity = gravity_on; }
	private:
		Body_2d(const Entity_types & type, const cgm::vec2 & pos, const float m, const AABB_2d & aabb);
		
		Entity_types	     m_type;
		float				 m_mass;
		cgm::vec2			 m_position;
		cgm::vec2			 m_velocity;
		cgm::vec2            m_vel_threshold;
		cgm::vec2			 m_acceleration;
		AABB_2d				 m_aabb;
		void *			     m_user_data;
		bool			     m_is_active = true;
		bool                 m_apply_gravity = true;
	};
}
#endif