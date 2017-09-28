#ifndef _BODY_2D_HPP
#define _BODY_2D_HPP
#include "vec2.hpp"
#include "AABB_2d.hpp"
#include "World.hpp"

/* Class that defines a physical entity that is affected by
 *  the physics engine
 */

//TODO: Mayby add a gravity scale...
namespace physics_2d {
	class Body_2d final {
	friend class World;
	public:
		World::Entity_types get_type()			const { return m_type;}
		cgm::vec2			get_position()      const { return m_position;}
		cgm::vec2			get_velocity()      const { return m_velocity;}
		cgm::vec2			get_acceleration()  const { return m_acceleration;}
		AABB_2d				get_aabb_2d()       const { return m_aabb; }

		void               *get_user_data()     const { return m_user_data; }
		void                set_user_data(void * pdata) { m_user_data = pdata; }
		
		void                add_force(const float f) { m_acceleration += f / m_mass; } // remember to add vector operations on cgm ....
	private:
		Body_2d(const World::Entity_types & type, const cgm::vec2 pos, const cgm::vec2 & vel, const float m, const AABB_2d & aabb);
		World::Entity_types  m_type;
		float				 m_mass;
		cgm::vec2			 m_position;
		cgm::vec2			 m_velocity;
		cgm::vec2			 m_acceleration;
		AABB_2d				 m_aabb;
		void *			     m_user_data;
		bool			     m_is_active = true;
	};
}
#endif