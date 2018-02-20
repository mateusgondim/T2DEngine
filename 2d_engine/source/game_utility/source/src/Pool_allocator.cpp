#include "Pool_allocator.hpp"
#include "memory_allocator.hpp"

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <iostream>

Pool_allocator::Pool_allocator(const std::size_t element_sz_bytes, const std::size_t num_elements, const std::size_t aligmnent) :
	m_element_sz_bytes(element_sz_bytes), m_alignment(aligmnent), m_pmemory(nullptr), m_ppfree_memory_list(nullptr)
{
	// element size must be at least the same size of void*
	assert(m_element_sz_bytes >= sizeof(void*));
	// element size must be a multiple of the alignment requirement
	assert(m_element_sz_bytes % m_alignment == 0);
	// alignment must be a power of two
	assert((m_alignment & (m_alignment - 1)) == 0);
	
	//calculate the size of the pool, including the extra bytes for the alignment
	m_pool_sz_bytes = (m_element_sz_bytes * num_elements) + aligmnent;
	
	//allocate the memory
	m_pmemory = allocate_aligned(m_element_sz_bytes * num_elements, m_alignment);
	if (m_pmemory == nullptr) { // out of memeory
		std::cerr << "ERROR  " << __FUNCTION__ << ": Unable to allocate " << std::endl;
	}
	else {//construct the free list 
		//cast the void* to void**, so the memory is 'interpret' as a buffer of void*
		m_ppfree_memory_list = static_cast<void**>(m_pmemory);
		// get the address of the end of the memory block
		std::uintptr_t end_address = reinterpret_cast<std::uintptr_t>(m_ppfree_memory_list) + (m_element_sz_bytes * num_elements);
		
		for (std::size_t element_cnt = 0; element_cnt <  num_elements; ++element_cnt) {
			//get the addresses of the current chunck and the next one
			std::uintptr_t curr_address = reinterpret_cast<std::uintptr_t>(m_ppfree_memory_list) + element_cnt * m_element_sz_bytes;
			std::uintptr_t next_address = curr_address + m_element_sz_bytes;
			//cast the address of the current chunk to a void**
			void **curr_memory = reinterpret_cast<void**>(curr_address);
			if(next_address >= end_address) { // last chunck
				*curr_memory = nullptr;
			}
			else {
				*curr_memory = reinterpret_cast<void*>(next_address);
			}
		}
	}

}

void* Pool_allocator::get_element() 
{
	if (m_pmemory == nullptr) {  
		std::cerr << "ERROR " << __FUNCTION__ << ": No memory was allocated to this pool" << std::endl;
		return nullptr;
	}
	
	if (m_ppfree_memory_list != nullptr) {
		void* pblock = reinterpret_cast<void*>(m_ppfree_memory_list);
		m_ppfree_memory_list = static_cast<void**>(*m_ppfree_memory_list);

		return pblock;
	}
	else { // out of memory blocks
		std::cerr << "ERROR " << __FUNCTION__ << ": out of memory blocks" << std::endl;
		return nullptr;
	}
}

void Pool_allocator::free_element(void* pblock) 
{
	if (m_pmemory == nullptr) {
		std::cerr << "ERROR " << __FUNCTION__ << ": No memory was allocated to this pool" << std::endl;
		return;
	}
	
	if (m_ppfree_memory_list == nullptr) { // the free list is empty
		m_ppfree_memory_list = reinterpret_cast<void**>(pblock);
		*m_ppfree_memory_list = nullptr;
	}
	else {
		void** ppreturned_block = m_ppfree_memory_list;
		m_ppfree_memory_list =  reinterpret_cast<void**>(pblock);
		*m_ppfree_memory_list = reinterpret_cast<void*>(ppreturned_block);
	}
	
}


void Pool_allocator::realease_pool_mem() 
{
	free_aligned(m_pmemory);
	m_pmemory = m_ppfree_memory_list = nullptr;
}

Pool_allocator::~Pool_allocator() 
{
	realease_pool_mem();
}

std::ostream & operator<<(std::ostream & os,  Pool_allocator & pool) 
{
	if (pool.m_pmemory == nullptr) {
		os << "ERROR " << __FUNCTION__ << ": No memory was allocated to this pool" << std::endl;
		return os;
	}
	
	std::size_t num_elements = (pool.m_pool_sz_bytes - pool.m_alignment) / pool.m_element_sz_bytes;
	os << "Pool memory info: total size of " << pool.m_pool_sz_bytes << " bytes| "
	   << pool.m_element_sz_bytes << " byte element size| " << num_elements << " elements| " << pool.m_alignment << " byte alignment requirement" << std::endl;
	os << "Printing Pool free memory list: "
	   << "memory address | contents(next pointer)" << std::endl;
	void **pplist = pool.m_ppfree_memory_list;
	for (std::size_t element_cnt = 0; element_cnt < num_elements; ++element_cnt) {
		if (pplist == nullptr) {
			break;
		}
		//prints the current element and the next pointer
		std::uintptr_t curr_address = reinterpret_cast<std::uintptr_t>(pplist);
		std::uintptr_t next_address = reinterpret_cast<std::uintptr_t>(*pplist);
		//moves to the next element
		pplist = static_cast< void**>(*pplist);
		os << "\t 0x" << std::hex << curr_address << " | 0x" << next_address << std::endl;
	}

	return os;
}