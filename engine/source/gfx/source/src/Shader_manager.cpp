#include "Shader_manager.hpp"
#include "Shader.hpp"
#include "Resource.hpp"
#include "Resource_manager.hpp"
#include "string_id.hpp"

#include <utility>
#include <string>

gfx::Shader_manager gfx::g_shader_mgr;

gfx::Shader_manager::Shader_manager() : rms::Resource_manager(std::string("shader"), sizeof(gfx::Shader), 5, 4) {}

rms::Resource *gfx::Shader_manager::create_impl(const char *name) 
{
	//get a block of memory from the pool for this resource
	void *pshader_mem = m_resources_mem_pool.get_element();
	
	//check if the allocation 
	if (pshader_mem == nullptr) {
		//error allocating 
		return nullptr;
	}
	else {
		//construct the object
		gfx::Shader *pshader_obj = new (pshader_mem) gfx::Shader(this, name);
		return pshader_obj;
	}
}

rms::Resource *gfx::Shader_manager::load(const char *name, const char *vert_fname, const char *frag_fname, const char *geom_fname) 
{
	std::pair<rms::Resource*, bool> create_or_retrieve_result = create_or_retrieve(name);

	if (create_or_retrieve_result.first != nullptr) {
		//set the shader's files
		gfx::Shader *pshader = static_cast<gfx::Shader*>(create_or_retrieve_result.first);
		pshader->set_vert_file_name(vert_fname);
		pshader->set_frag_file_name(frag_fname);
		if (geom_fname != nullptr) {
			pshader->set_geom_file_name(geom_fname);
		}
		pshader->load();

		return pshader;
	}
	else {
		return nullptr;
	}
}