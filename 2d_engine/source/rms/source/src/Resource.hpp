#ifndef _RESOURCE_HPP
#define _RESOURCE_HPP

/* Resource: part of the resource management system(rms), this abstract class represents a loadable 
 * resource, such as texture, shaders, sounds etc. Resources are data objects that must be loaded and managed
 * by the application. The key thing, is that they must be identified by an unique ID, must be loaded only once and
 * should be retrieved efficiently
 */

#include "string_id.hpp"

namespace rms { class Resource_manager; }

namespace rms {
	class Resource {
	public:
		Resource(Resource_manager *pcreator, const char *name);
		
		//should not be overloaded, a resource's deallocation code should reside in the unload function
		virtual ~Resource();

		virtual size_t get_size() const { return m_size; }
		virtual const char *get_name() const;
		
		string_id get_id() const { return m_id; }

		virtual Resource_manager *get_creator() { return m_pcreator; }
		
		virtual void    load() = 0;
		virtual void    unload() = 0;
	protected:
		Resource();
		
		virtual size_t  calculate_size() const = 0;

		Resource_manager *m_pcreator;
		string_id		  m_id;
		size_t			  m_size;
	};
}

#endif