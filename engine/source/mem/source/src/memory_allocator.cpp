#include "memory_allocator.hpp"

#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <cstdint>

namespace mem {

	/*allocate_unaligned: Attempts to allocate a unaligned block of 'size_bytes' bytes,
	 *returns nullptr in case of failure
	 */
	void *allocate_unaligned(const std::size_t size_bytes)
	{
		void * pmemory_block = malloc(size_bytes);

		return pmemory_block;
	}

	void free_unaligned(void *pmemory)
	{
		free(pmemory);
	}

	/* allocate_aligned: Attempts to allocate a block of 'size_bytes' bytes,
	 * with 'alignment' bytes aligment requirent. IMPORTANT: 'alignment' must be a power of 2.
	 * returns nullptr in case of failure
	 */
	void * allocate_aligned(const  std::size_t size_bytes, const std::size_t alignment)
	{
		assert(alignment >= 1);
		assert(alignment <= 128);
		assert((alignment & (alignment - 1)) == 0); // pwr of 2

		//Increase the memory to be allocated to be able to align it 
		std::size_t expanded_size_bytes = size_bytes + alignment;

		//allocate the expanded unaligned block an convert it to uintptr_t
		std::uintptr_t raw_address = reinterpret_cast<std::uintptr_t>(allocate_unaligned(expanded_size_bytes));

		//Calculate the adjustment by masking off the lower bits
		//of the address, to determine how 'misaligned it is
		std::size_t     mask = alignment - 1;
		std::uintptr_t  misalignment = raw_address & mask;
		std::ptrdiff_t  adjustment = alignment - misalignment;

		//Calculate the ajusted address
		std::uintptr_t aligned_address = raw_address + adjustment;

		// store the adjustment in the byte immediately 
		// preceding the ajusted address
		assert(adjustment < 256);
		std::uint8_t *paligned_mem = reinterpret_cast<std::uint8_t *>(aligned_address);
		paligned_mem[-1] = static_cast<std::uint8_t>(adjustment);

		return static_cast<void*>(paligned_mem);
	}

	void free_aligned(void * pmemory)
	{
		const std::uint8_t *paligned_memory = reinterpret_cast<const std::uint8_t*>(pmemory);
		std::uintptr_t aligned_address = reinterpret_cast<std::uintptr_t>(pmemory);
		std::ptrdiff_t ajustment = static_cast<std::ptrdiff_t>(paligned_memory[-1]);

		std::uintptr_t raw_address = aligned_address - ajustment;
		void * praw_memory = reinterpret_cast<void*>(raw_address);

		free_unaligned(praw_memory);
	}

}