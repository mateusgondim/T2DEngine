#ifndef _MEMORY_ALLOCATOR_HPP
#define _MEMORY_ALLOCATOR_HPP
#include <cstddef>

namespace mem {

	//memory_allocator: Basic allocation functions to return aligned and unaligned memory blocks
	void* allocate_unaligned(const std::size_t size_bytes);
	void free_unaligned(void *pmemory);

	void* allocate_aligned(const  std::size_t size_bytes, const std::size_t alignment);
	void  free_aligned(void * pmemory);
}
#endif // !_MEMORY_ALLOCATOR_HPP
