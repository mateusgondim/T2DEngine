#include "Broadphase.hpp"
#include "AABB_2d.hpp"
#include "Pool_allocator.hpp"
#include <cstdlib>
#include <cstddef>
#include <cstdint>

namespace physics_2d {
	Broadphase::Broadphase() 
		: m_pair_buff_capacity(MAX_NUM_PAIRS), m_pair_cnt(0), m_proxy_hash_table_capacity(MAX_NUM_PROXIES), m_proxy_cnt(0), m_move_capacity(MAX_NUM_PROXIES), m_move_cnt(0)
	{
		//set up the pair's  buffer
		m_ppair_buffer = static_cast<Proxy_pair*>(malloc(sizeof(Proxy_pair) * m_pair_buff_capacity));

		//set up hash table
		m_pproxy_hash_table = static_cast<Proxy_hash_entry*>(malloc(sizeof(Proxy_hash_entry) * m_proxy_hash_table_capacity));
		int32_t i;
		for (i = 0; i != m_proxy_hash_table_capacity - 1; ++i) {
			m_pproxy_hash_table[i].user_data = nullptr;
			m_pproxy_hash_table[i].next = i + 1;
		}
		//last entry
		m_pproxy_hash_table[i].user_data = nullptr;
		m_pproxy_hash_table[i].next = NULL_PROXY_ID;

		//first free proxy entry
		m_next_free_entry = 0;

		//set move buffer
		m_pmove_buffer  =  static_cast<int32_t*>(malloc(sizeof(int32_t) * m_move_capacity));
	}

	Broadphase::~Broadphase() 
	{
		//destroy pair buffer
		free(static_cast<void*>(m_ppair_buffer));
		m_ppair_buffer = nullptr;
		//free hash memory
		free(static_cast<void*>(m_pproxy_hash_table));
		m_pproxy_hash_table = nullptr;

		//destroy move buffer
		free(static_cast<void*>(m_pmove_buffer));
		m_pmove_buffer = nullptr;

		m_move_cnt = m_pair_cnt = m_proxy_cnt = 0;
	}

	int32_t Broadphase::store_proxy_in_hash(const AABB_2d & aabb, void *user_data)
	{
		//get the next free entry on the hash table
		int32_t proxy_id = m_next_free_entry;
		Proxy_hash_entry *pproxy = &m_pproxy_hash_table[m_next_free_entry];
		m_next_free_entry = pproxy->next;

		//set proxy's data
		pproxy->user_data = user_data; // Collider_2d_proxy*
		pproxy->aabb	  = aabb;
		pproxy->next	  = NULL_PROXY_ID;

		//new proxy added, update move buffer
		m_pmove_buffer[m_move_cnt++] = proxy_id;
		
		//increment proxy count
		++m_proxy_cnt;
		
		return proxy_id;
	}

	void Broadphase::remove_proxy_from_hash(const int32_t proxy_id)
	{
		assert(proxy_id >= 0 && proxy_id < m_proxy_hash_table_capacity);
		
		// "free" hash entry space
		m_pproxy_hash_table[proxy_id].user_data = nullptr;
		m_pproxy_hash_table[proxy_id].next = m_next_free_entry;
		m_next_free_entry = proxy_id;

		// remove from move buffer, if present
		for (size_t i = 0; i != m_move_cnt; ++i) {
			if (m_pmove_buffer[i] == proxy_id) {
				m_pmove_buffer[i] = NULL_PROXY_ID;
				break;
			}
		}

	}
}