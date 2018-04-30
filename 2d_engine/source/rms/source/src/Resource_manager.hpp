#ifndef _RESOURCE_MANAGER_HPP
#define _RESOURCE_MANAGER_HPP


/* A resource manager is responsible for managing a pool or resources of a particular type. 
 * It must index them, look them up, load and destroy them.
 *
 */

#include "Pool_allocator.hpp"
#include "string_id.hpp"
#include <utility>
#include <string>
#include <map>

namespace rms { class Resource; }

namespace rms {
	class Resource_manager {
	public:
		typedef std::map<string_id, Resource*> resource_map;

		Resource_manager(std::string & resource_type, const std::size_t element_sz_bytes, const std::size_t num_elements, const std::size_t aligmnent);
		virtual ~Resource_manager();

		virtual std::pair<Resource*, bool> create_or_retrieve(const char *name);
		virtual Resource* create(const char *name);
		virtual size_t get_memory_budget() const;
		
		//loading functions
		//in our case unloading = removing
		virtual void unload(const char *name);
		virtual void unload(const string_id id);
		virtual void unload_all();

		virtual Resource *get_by_name(const char *name);
		virtual Resource *get_by_id(const string_id id);

		//loading functions
		virtual Resource *load(const char *name);

		const std::string & get_resource_type() const { return m_resource_type; }
		
		//memory functions
			//Pool_allocator *get_resource_pool();
			//void			destroy_resource_pool();
	protected:
		virtual Resource *create_impl(const char *name) = 0;
		Pool_allocator  m_resources_mem_pool;
		resource_map    m_resources;
		size_t		    m_memory_budget;
		std::string	    m_resource_type;
	};
}

#endif // !_RESOURCE_MANAGER_HPP
