#ifndef _BODY_2D_HPP
#define _BODY_2D_HPP
#include "vec2.hpp"
#include "AABB_2d.hpp"
#include "Collider_2d.hpp"

/* Class that defines a physical entity that is affected by
 *  the physics engine
 */

//TODO: Mayby add a gravity scale...
// A (0, 0) vel_threshold means object can accelerate to infinity

namespace physics_2d { struct Body_2d_def;  struct Collider_2d_def; class Collider_2d; }
namespace physics_2d {
	class Body_2d final {
	friend class World;
	public:
		enum Entity_types {STATIC, KINEMATIC, DYNAMIC };

		       Entity_types	  get_type()		  const;
		const  math::vec2  &  get_position()	  const;
		const  math::vec2  &  get_velocity()      const;
		const  math::vec2  &  get_acceleration()  const;
		//const  AABB_2d     &  get_aabb()          const;
			   Collider_2d   *get_collider();
		const  Collider_2d   *get_collider()	  const;
			   float		  get_mass()		  const;
			   bool			  is_active()		  const;
			   void           *get_user_data()    const;
			   World          *get_world();
			   Body_2d		  *get_next();

			   Collider_2d   *create_collider_2d(const Collider_2d_def & coll_def);
			   void			  destroy_collider_2d();
			   void			  translate_by(const math::vec2 & t);
			   void			  set_gravity_scale(const float k);
		       void	          set_position(const math::vec2 & wld_pos);
			   void           set_velocity(const math::vec2 & velocity);
			   void			  set_acceleration(const math::vec2 & acceleration);
			 //  void			  set_aabb(const AABB_2d & aabb);
			   void			  set_active(const bool is_active); 
			   void           set_user_data(void * pdata);
		

		void                set_velocity_threshold(const math::vec2 & vel_threshold) { m_vel_threshold = vel_threshold; }
		void                add_to_velocity(const math::vec2 & vel); 
		void                stop_movement_x() { m_velocity.x = m_acceleration.x = 0.0f; }
		void                stop_movement_y() { m_velocity.y = m_acceleration.y = 0.0f; }
		
		~Body_2d();
	private:
		//Body_2d(const Entity_types & type, const math::vec2 & pos, const float m, const AABB_2d & aabb);
		Body_2d(const Body_2d_def *pbody_def, World *pworld);
		Entity_types	     m_type;
		float				 m_mass;
		float				 m_gravity_scale;
		math::vec2			 m_position;
		math::vec2			 m_velocity;
		math::vec2           m_vel_threshold;
		math::vec2			 m_acceleration;
		//AABB_2d				 m_aabb;
		Collider_2d			*m_pcollider;

		void *			     m_puser_data;
		World				*m_pworld;
		Body_2d				*m_pprev;  // pointer to the previous Body_2d object inside the world's body list
		Body_2d				*m_pnext;
		bool			     m_is_active = true;
		bool				 m_map_collision;
	};

	inline Body_2d::Entity_types Body_2d::get_type() const 
	{
		return m_type;
	}

	inline const math::vec2 & Body_2d::get_position() const 
	{ 
		return m_position; 
	}

	inline const math::vec2 & Body_2d::get_velocity() const 
	{
		return m_velocity;
	}

	inline const math::vec2 & Body_2d::get_acceleration() const 
	{
		return m_acceleration;
	}

	inline Collider_2d *Body_2d::get_collider() 
	{
		return m_pcollider;
	}

	inline const Collider_2d *Body_2d::get_collider() const 
	{
		return m_pcollider;
	}

	/*
	inline const AABB_2d & Body_2d::get_aabb() const 
	{
		return m_aabb;
	}
	*/
	inline float Body_2d::get_mass() const 
	{
		return m_mass;
	}

	inline bool Body_2d::is_active() const 
	{
		return m_is_active;
	}

	inline void * Body_2d::get_user_data() const 
	{
		return m_puser_data;
	}

	inline World *Body_2d::get_world() 
	{
		return m_pworld;
	}

	inline Body_2d *Body_2d::get_next() 
	{
		return m_pnext;
	}

	inline void Body_2d::set_gravity_scale(const float k) 
	{
		m_gravity_scale = k;
	}

	//sets the position and changes the aab accordingly
	inline void Body_2d::set_position(const math::vec2 & wld_pos) 
	{
		math::vec2 t = wld_pos - m_position;
		
		if (m_pcollider) {
			m_pcollider->m_aabb.p_max += t;
			m_pcollider->m_aabb.p_min += t;
		}
		if (m_pcollider->m_pproxy) {
			m_pcollider->m_pproxy->aabb.p_max += t;
			m_pcollider->m_pproxy->aabb.p_min += t;
		}

		//m_aabb.p_max += t;
		//m_aabb.p_min += t;
		m_position = wld_pos;
	}

	inline void Body_2d::translate_by(const math::vec2 & t) 
	{
		m_position    += t;
		if (m_pcollider) {
			m_pcollider->m_aabb.p_max += t;
			m_pcollider->m_aabb.p_min += t;
		}
		if (m_pcollider->m_pproxy) {
			m_pcollider->m_pproxy->aabb.p_max += t;
			m_pcollider->m_pproxy->aabb.p_min += t;
		}
		//m_aabb.p_max  += t;
		//m_aabb.p_min  += t;
	}

	inline void Body_2d::set_velocity(const math::vec2 & velocity) 
	{
		m_velocity = velocity;
	}

	inline void Body_2d::add_to_velocity(const math::vec2 & vel) 
	{
		m_velocity += vel;
	}

	inline void Body_2d::set_acceleration(const math::vec2 & acceleration) 
	{
		m_acceleration = acceleration;
	}
	
	/*inline void Body_2d::set_aabb(const AABB_2d & aabb) 
	{
		m_aabb = aabb;
	}*/


	inline void Body_2d::set_user_data(void *pdata) 
	{
		m_puser_data = pdata;
	}
}
#endif