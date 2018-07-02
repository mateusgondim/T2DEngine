#include "Creator.hpp"
#include "runtime_memory_allocator.hpp"
#include "Body_2d_def.hpp"

namespace gom {
	Creator::~Creator() 
	{
		mem::free(static_cast<void*>(m_pbody_def), sizeof(physics_2d::Body_2d_def));
	}
}