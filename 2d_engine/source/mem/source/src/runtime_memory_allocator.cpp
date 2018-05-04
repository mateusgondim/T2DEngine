#include "runtime_memory_allocator.hpp"
#include "Pool_allocator.hpp"
#include <cstdlib>

namespace mem {
	// internal variables
	static const unsigned s_BLOCK_SIZES[] = {
		// 4- increment
		4,  8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48,
		52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96,
		// 32 - increment
		128, 160, 192, 224, 256, 288, 320, 352, 384,
		416, 448, 480, 512, 544, 576, 608, 640
	};

	static const unsigned s_POOL_SIZE = 8192;
	static const unsigned s_ALIGNMENT = 4;
	static const size_t   s_NUM_BLOCK_SIZES = sizeof(s_BLOCK_SIZES) / sizeof(s_BLOCK_SIZES[0]);
	static const size_t   s_MAX_BLOCK_SIZE = s_BLOCK_SIZES[s_NUM_BLOCK_SIZES - 1];

	static unsigned *s_pblock_sz_lookup = nullptr;
	static Pool_allocator s_allocators[s_NUM_BLOCK_SIZES];

	//internal helper function, lookup the allocator that is big enough to hold elements of size 'sz'
	static Pool_allocator *lookup(const size_t sz) 
	{
		static bool s_initialized = false;
		if (!s_initialized) {
			//initialize block size lookup
			s_pblock_sz_lookup = new unsigned[s_MAX_BLOCK_SIZE + 1];
			unsigned j = 0;
			for (unsigned i = 0; i <= s_MAX_BLOCK_SIZE; ++i) {
				if (i > s_BLOCK_SIZES[j]) {
					++j;
				}
				s_pblock_sz_lookup[i] = j;
			}
			//initialize pool allocators
			for (size_t i = 0; i < s_NUM_BLOCK_SIZES; ++i) {
				s_allocators[i].alloc_pool(s_BLOCK_SIZES[i], s_POOL_SIZE / s_BLOCK_SIZES[i], s_ALIGNMENT);
			}
			s_initialized = true;
		}

		if (sz > s_MAX_BLOCK_SIZE) { //block is to big to use pool allocators
			return nullptr;
		}
		else {
			return s_allocators + s_pblock_sz_lookup[sz];
		}
	}

	void *allocate(const size_t sz) 
	{
		Pool_allocator *pallocator = lookup(sz);

		//check if the block size w
		if (pallocator != nullptr) {
			return pallocator->get_element();
		}
		else {
			return std::malloc(sz);
		}
	}

	void free(void *pmem, const size_t sz) 
	{
		if (sz > s_MAX_BLOCK_SIZE) {
			//it was allocated using std::malloc
			std::free(pmem);
		}
		else {
			Pool_allocator *pallocator = lookup(sz);
			pallocator->free_element(pmem);
		}
	}
}