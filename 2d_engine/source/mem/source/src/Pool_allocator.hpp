#ifndef _POOL_ALLOCATOR_HPP
#define _POOL_ALLOCATOR_HPP

#include <cstddef>
#include <iostream>
namespace mem {
	class Pool_allocator final {
		friend std::ostream & operator<<(std::ostream & os, Pool_allocator & pool);
	public:
		Pool_allocator();
		Pool_allocator(const std::size_t element_sz_bytes, const std::size_t num_elements, const std::size_t aligmnent);

		Pool_allocator(const Pool_allocator &alloc) = delete; //copy constructor
		Pool_allocator & operator=(const Pool_allocator &rhs) = delete; // copy-assignment operator
		Pool_allocator(Pool_allocator &&alloc) = delete;//move constructor
		Pool_allocator & operator=(Pool_allocator && rhs) = delete; // move-assignment operator

		~Pool_allocator();

		void alloc_pool(const std::size_t element_sz_bytes, const std::size_t num_elements, const std::size_t aligmnent);

		void *get_element();
		void  free_element(void *pelement);
		//void  allocate_pool();

		std::size_t	get_element_size() const;
	private:
		void  realease_pool_mem();

		std::size_t  m_pool_sz_bytes;    //the size in bytes of the pool
		std::size_t  m_element_sz_bytes; // size in bytes of each element
		std::size_t  m_alignment;         // the element/pool alignment requirement in bytes
		void*        m_pmemory;           // pointer to the first address of the pool, used to relase all the memory
		void**       m_ppfree_memory_list; // pointer to pointer, used to point to the head of the free list
	};

	std::ostream & operator<<(std::ostream & os, Pool_allocator & pool);
}
#endif // !_POOL_ALLOCATOR_HPP
