#ifndef _N_SQUARED_BROADPHASE_HPP
#define _N_SQUARED_BROADPHASE_HPP

#include "Broadphase.hpp"
#include "Pool_allocator.hpp"
#include <cstddef>
#include <cstdint>

namespace physics_2d { struct AABB_2d; }

namespace physics_2d {
	
	struct Proxy_node {
		Proxy_node  *pnext;
		int32_t		 id;
	};
	
	class N_squared_broadphase : public Broadphase {
	public:
		N_squared_broadphase();
		virtual   int32_t	   create_proxy(const AABB_2d & aabb, void *user_data) override;
		virtual   void	       destroy_proxy(const int32_t proxy_id) override;
		virtual   Proxy_pair  *compute_pairs() override;
		virtual	  void	       update() override;
	private:
		//linked list of created proxies
		Proxy_node		     *m_pproxy_list;
		std::size_t		      m_node_cnt;
		mem::Pool_allocator   m_node_allocator;
	};
}

#endif // !_N_SQUARED_BROADPHASE_HPP
