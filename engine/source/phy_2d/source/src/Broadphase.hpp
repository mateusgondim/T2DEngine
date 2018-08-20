#ifndef _BROADPHASE_HPP
#define _BROADPHASE_HPP

#include <cstddef>
#include <cstdint>
#include <cassert>
#include "AABB_2d.hpp"

#define  MAX_NUM_PROXIES 40
#define  MAX_NUM_PAIRS   80

namespace mem { class Pool_allocator; }

namespace physics_2d {
	/* Basic interface to implement a physics's engine broad phase. This class keeps a hash map with a proxy entry for each
	 * collider_2d in the world, those entries are proecessed in the Broadphase for aabb collision detection. The Broadphase
	 * should populate the Proxy_pair buffer with each pair of possible colliding entities.
	 *  Every new collider_2d added to the World should call Broadphase::create_proxy, so it can be stored in the Broadphase's hashmap 
	 * 
	 */

	struct Proxy_pair {
		int32_t a_id;
		int32_t b_id;
	};

	struct Proxy_hash_entry {
		void		*user_data; // allways Collider_2d_proxy*
		AABB_2d		aabb;
		int32_t     next;
	};

	enum {
		NULL_PROXY_ID = -1
	};

	class Broadphase {
	public:
		Broadphase();
		virtual int32_t   create_proxy(const AABB_2d & aabb, void *user_data) = 0;
		virtual void	  destroy_proxy(const int32_t proxy_id) = 0;
		void			  move_proxy(const int32_t proxy_id);
		void			  clear_move_buffer();
		size_t            get_move_buffer(const int32_t **pmove_buff) const;
		void	         *get_user_data(const int32_t proxy_id);
		size_t			  get_proxy_count() const;
		size_t			  get_pair_count() const;
		virtual		      Proxy_pair *compute_pairs() = 0;
		virtual			  void		  update() = 0;
		
		virtual  ~Broadphase();

	protected:
		int32_t store_proxy_in_hash(const AABB_2d & aabb, void *user_data);
		void    remove_proxy_from_hash(const int32_t proxy_id);
		Proxy_hash_entry & get_proxy_by_id(const int32_t id);

		//buffer for proxy pairs that may be colliding
		Proxy_pair       *m_ppair_buffer;
		std::size_t		  m_pair_buff_capacity;
		std::size_t		  m_pair_cnt;

	private:

		//hash table 
		Proxy_hash_entry  *m_pproxy_hash_table;
		std::size_t		   m_proxy_hash_table_capacity;
		std::size_t		   m_next_free_entry;

		std::size_t		   m_proxy_cnt;

		//Move buffer, used to store the id of all the proxies that moved in the previous frame
		// so should be checked againd the other proxies
		int32_t			   *m_pmove_buffer;
		size_t				m_move_capacity;
		size_t				m_move_cnt;
	};

	inline void * Broadphase::get_user_data(const int32_t proxy_id) 
	{
		assert(proxy_id >= 0 && proxy_id < m_proxy_hash_table_capacity);
		return m_pproxy_hash_table[proxy_id].user_data;
	}

	inline size_t Broadphase::get_proxy_count() const 
	{
		return m_proxy_cnt;
	}

	inline size_t Broadphase::get_pair_count() const 
	{
		return m_pair_cnt;
	}

	inline void Broadphase::move_proxy(const int32_t proxy_id) 
	{
		assert(proxy_id >= 0 && proxy_id < m_proxy_hash_table_capacity);
		m_pmove_buffer[m_move_cnt++] = proxy_id;
	}

	inline Proxy_hash_entry & Broadphase::get_proxy_by_id(const int32_t id)
	{
		assert(id >= 0 && id < m_proxy_hash_table_capacity);
		return m_pproxy_hash_table[id];
	}

	inline void Broadphase::clear_move_buffer() 
	{
		m_move_cnt = 0;
	}

	// get the id of all the proxies that moved in the last step
	// returns the size
	inline size_t Broadphase::get_move_buffer(const int32_t **pmove_buff) const  
	{
		*pmove_buff = m_pmove_buffer;
		return m_move_cnt;
	}
}

#endif // !_BROADPHASE_HPP
