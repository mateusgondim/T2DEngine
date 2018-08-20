#include "Collider_2d.hpp"
#include "Collider_2d_def.hpp"
#include "Body_2d.hpp"
#include "Broadphase.hpp"
#include "Pool_allocator.hpp"

#include <cstdint>

namespace physics_2d {

	Collider_2d::Collider_2d() : m_pbody(nullptr), m_pproxy(nullptr), m_is_trigger(false) {}

	void Collider_2d::Create(Body_2d *pbody, const Collider_2d_def *pdef, mem::Pool_allocator & proxy_allocator) 
	{
		m_pbody		  =  pbody;
		m_aabb        =  pdef->m_aabb;
		m_is_trigger  =  pdef->m_is_trigger;
		
		//create proxy to connect with broad-phase
		void *pmem = proxy_allocator.get_element();
		if (pmem) {
			m_pproxy = static_cast<Collider_2d_proxy*>(new (pmem) Collider_2d_proxy);
			m_pproxy->pcollider = this;
			m_pproxy->proxy_id = NULL_PROXY_ID;
		}
	}

	void Collider_2d::Destroy(mem::Pool_allocator & proxy_alloc) 
	{
		//Must first call destroy_proxy to remove it from the broadphase

		//deallocate proxy data member
		proxy_alloc.free_element(static_cast<void*>(m_pproxy));
		m_pproxy = nullptr;
		m_pbody  = nullptr;
	}

	void Collider_2d::Create_proxy(Broadphase & broadphase) 
	{
		int32_t id = broadphase.create_proxy(m_aabb, m_pproxy);
		m_pproxy->proxy_id = id;
		m_pproxy->aabb = m_aabb;
	}

	void Collider_2d::destroy_proxy(Broadphase & broadphase) 
	{
		if (m_pproxy->proxy_id != NULL_PROXY_ID) {
			broadphase.destroy_proxy(m_pproxy->proxy_id);
			m_pproxy->proxy_id = NULL_PROXY_ID;
		}
	}
}