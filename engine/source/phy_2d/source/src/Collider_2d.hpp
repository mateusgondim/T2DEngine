#ifndef _COLLIDER_2D_HPP
#define _COLLIDER_2D_HPP

#include "AABB_2d.hpp"
#include <cstdint>

namespace physics_2d { class Broadphase; class Body_2d; struct Collider_2d_def;  class Collider_2d; }
namespace mem { class Pool_allocator; }
namespace physics_2d {

	// Proxy is used to connect Colliders_2d to the Broad-phase
	struct Collider_2d_proxy {
		AABB_2d		  aabb;
		Collider_2d  *pcollider;
		int32_t		  proxy_id;
	};


	class Collider_2d {
	friend class Body_2d;
	public:
		Collider_2d();

		void set_trigger(const bool trigger);
		bool is_trigger() const;
		int32_t get_proxy_id() const;
		Body_2d *get_body();
		AABB_2d & get_aabb();
	const AABB_2d & get_aabb() const;
	private:
		void Create(Body_2d *pbody, const Collider_2d_def * pdef, mem::Pool_allocator & proxy_alloc);
		void Destroy(mem::Pool_allocator & proxy_alloc);

		void Create_proxy(Broadphase & broadphase);
		void destroy_proxy(Broadphase & broadphase);

		Body_2d			   *m_pbody;
		AABB_2d			    m_aabb;
		Collider_2d_proxy  *m_pproxy;
		bool			    m_is_trigger;
	};

	inline int32_t Collider_2d::get_proxy_id() const 
	{
		if (m_pproxy) {
			return m_pproxy->proxy_id;
		}
		else {
			return -1;
		}
	}

	inline void Collider_2d::set_trigger(const bool trigger) 
	{
		m_is_trigger = trigger;
	}

	inline bool Collider_2d::is_trigger() const 
	{
		return m_is_trigger;
	}
	
	inline Body_2d *Collider_2d::get_body() 
	{
		return m_pbody;
	}

	inline AABB_2d & Collider_2d::get_aabb() 
	{
		return m_aabb;
	}
	
	inline const AABB_2d & Collider_2d::get_aabb() const 
	{
		return m_aabb;
	}
}

#endif // !_COLLIDER_2D_HPP
