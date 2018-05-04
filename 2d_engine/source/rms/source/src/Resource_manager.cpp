#include "Resource_manager.hpp"
#include "Resource.hpp"
#include "string_id.hpp"
#include "Pool_allocator.hpp"

#include <utility>
#include <iostream>

// MAYBE WE SHOULD IMPLEMENT THE CREATE FUNCTION, TO SHIELD THE SUBCLASS OF WORKING WITH THE RESOURCE'S MAP...

namespace rms {

	Resource_manager::Resource_manager(std::string & resource_type, const std::size_t element_sz_bytes, const std::size_t num_elements, const std::size_t aligmnent) 
		: m_resource_type(resource_type), m_resources_mem_pool(element_sz_bytes, num_elements, aligmnent), m_memory_budget(element_sz_bytes * num_elements) {}

	Resource_manager::~Resource_manager() 
	{
		unload_all();
		//MEMORY POOL DESTRUCTOR IS CALLED IMPLICITLY
	}

	Resource *Resource_manager::create(const char *name) 
	{
		//call the criation implementation. this Function should create the specific Resource of this manager
		Resource *presource = create_impl(name);

		//if the resource was created succefully, add it to the resouces map
		if (presource != nullptr) {
			//check for name collision
			const string_id id = presource->get_id();
			resource_map::iterator it = m_resources.find(id);
			
			//if there is no resource with this id
			if (it == m_resources.end()) {
				m_resources[id] = presource;
				return presource;
			}
			else {
				//invalid id
				std::cerr << "ERROR(" << __FUNCTION__ << "): Collision with resource id = " << id << " generated for the name " << name << std::endl;
				//dealocate resource
				m_resources_mem_pool.free_element(static_cast<void*>(presource));
				return nullptr;

			}
		}
		else {
			//could not create resource
			std::cerr << "ERROR(" << __FUNCTION__ << "Could not create resource" << std::endl;
			return nullptr;
		}
	}

	std::pair<Resource*, bool> Resource_manager::create_or_retrieve(const char *name) 
	{
		//try to retrieve resource by name
		bool created = false;
		Resource *presource = get_by_name(name);
		if (presource == nullptr) {
			//resource does not exist, try to create it
			created = true;
			presource = create(name);
		}
		return std::pair<Resource*, bool>(presource, created);
	}


	Resource *Resource_manager::load(const char *name) 
	{
		std::pair<Resource*, bool> create_or_retrieve_result = create_or_retrieve(name);
		create_or_retrieve_result.first->load();
		return create_or_retrieve_result.first;
	}
	
	
	// NOT REMOVING THE ID FROM THE STRING TABLE
	void Resource_manager::unload(const char *name) 
	{
		Resource *presource = get_by_name(name);
		
		//if it exists, unload it
		if (presource != nullptr) {
			string_id id = presource->get_id();

			//deallocate resource
			presource->unload();
			
			//Remove from the resource map
			m_resources.erase(id);

			//return memory to pooll
			m_resources_mem_pool.free_element(static_cast<void*>(presource));
		}
	}
	
	void Resource_manager::unload(const string_id id) 
	{
		Resource *presource = get_by_id(id);

		//if it exists, unload it
		if (presource != nullptr) {
			//deallocate resource
			presource->unload();

			//Remove from the resource map
			m_resources.erase(id);

			//return memory to pooll
			m_resources_mem_pool.free_element(static_cast<void*>(presource));
		}

	}
	
	void Resource_manager::unload_all() 
	{
		resource_map::iterator it;

		for (it = m_resources.begin(); it != m_resources.end(); ++it) {
			//deallocate resource
			it->second->unload();

			//return memory to pool
			m_resources_mem_pool.free_element(static_cast<void*>(it->second));
		}
		m_resources.clear();
	}

	size_t Resource_manager::get_memory_budget() const 
	{
		return m_memory_budget;
	}

	Resource *Resource_manager::get_by_name(const char *name) 
	{
		//get the id from the resource name
		const string_id id = intern_string(name);

		//search for a resource with the id
		resource_map::iterator it = m_resources.find(id);
		if (it != m_resources.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	Resource *Resource_manager::get_by_id(const string_id id) 
	{
		//search for a resource with the id
		resource_map::iterator it = m_resources.find(id);
		if (it != m_resources.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

}