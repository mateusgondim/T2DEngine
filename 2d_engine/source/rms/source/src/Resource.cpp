#include "Resource.hpp"

#include "Resource_manager.hpp"
#include "string_id.hpp"

namespace rms {
	Resource::Resource() : m_pcreator(nullptr) {}

	Resource::Resource(Resource_manager *pcreator, const char *name) : m_pcreator(pcreator) 
	{
		m_id = intern_string(name);
	}

	Resource::~Resource() {}

	const char *Resource::get_name() const 
	{
		return get_string_by_id(m_id);
	}


}