#include "Broadphase.hpp"
#include "N_squared_broadphase.hpp"
#include "Collider_2d.hpp"
#include "AABB_2d.hpp"
#include "Pool_allocator.hpp"

#include <iostream>
#include <cstdint>
#include <cstddef>

namespace physics_2d {
	N_squared_broadphase::N_squared_broadphase() 
		: Broadphase(), m_node_cnt(0), m_pproxy_list(nullptr), m_node_allocator(sizeof(Proxy_node), MAX_NUM_PROXIES, 4) {}

	int32_t N_squared_broadphase::create_proxy(const AABB_2d & aabb, void *user_data) 
	{
		// store  proxy in the hash table and get its key/id
		int32_t id = store_proxy_in_hash(aabb, user_data);

		//add proxy to the list
		void * pmem = m_node_allocator.get_element();

		if (pmem == nullptr) {
#ifndef NDEBUG
			std::cerr << "ERROR (" << __FUNCTION__ << "): UNABLE TO ALLOCATE MEMORY FOR PROXY_NODE" << std::endl;
#endif // !NDEBUG
			return id;
		}

		Proxy_node *pnode = static_cast<Proxy_node*>( new (pmem) Proxy_node);
		pnode->id = id;
		pnode ->pnext = m_pproxy_list;
		m_pproxy_list = pnode;
		++m_node_cnt;

		return id;
	}

	void N_squared_broadphase::destroy_proxy(const int32_t proxy_id) 
	{
		//remove proxy from the hash table
		remove_proxy_from_hash(proxy_id);

		//remove from the list
		Proxy_node *pcurrent = m_pproxy_list;
		Proxy_node *pprev = nullptr;
		for (; pcurrent != nullptr; pprev = pcurrent, pcurrent = pcurrent->pnext) {
			if (pcurrent->id == proxy_id) {
				//id found, remove from the list
				if (pprev) {
					pprev->pnext = pcurrent->pnext;
				}
				else {
					m_pproxy_list = pcurrent->pnext;
				}
				//deallocate
				m_node_allocator.free_element(static_cast<void*>(pcurrent));
				--m_node_cnt;
				break;
			}
		}
	}
	// IS IT POSSIBLE TO GET DUPLICATE PAIRS???????????????????????????????????????????????????????????
	Proxy_pair *N_squared_broadphase::compute_pairs() 
	{
		const int32_t *pmove_buff;
		size_t	       move_cnt;

		//clear pair buffer
		m_pair_cnt = 0;

		//get the buffer with the ids of all the proxies that need to be checked for collision
		move_cnt = get_move_buffer(&pmove_buff);
		for (size_t i = 0; i != move_cnt; ++i) {
			//get the AABB of this proxy
			int32_t proxy_a_id = pmove_buff[i];
			
			if (proxy_a_id == NULL_PROXY_ID) {
				continue;
			}

			const AABB_2d  & first_coll_aabb = static_cast<Collider_2d_proxy*>(get_user_data(proxy_a_id))->aabb;

			//loop through all the colliders in the World, using their collider proxy
			for (Proxy_node *pnode = m_pproxy_list; pnode != nullptr; pnode = pnode->pnext) {
				int32_t proxy_b_id = pnode->id;

				if (proxy_a_id == proxy_b_id) {
					continue;
				}

				const AABB_2d & second_coll_aabb = static_cast<Collider_2d_proxy*>(get_user_data(proxy_b_id))->aabb;
				if (test_overlap(first_coll_aabb, second_coll_aabb) ) {
					//make pair
					m_ppair_buffer[m_pair_cnt].a_id = proxy_a_id;
					m_ppair_buffer[m_pair_cnt].b_id = proxy_b_id;
					++m_pair_cnt;
				}
			}
		}

		clear_move_buffer();

		return m_ppair_buffer;
	}

	void N_squared_broadphase::update() 
	{
		return;
	}
}