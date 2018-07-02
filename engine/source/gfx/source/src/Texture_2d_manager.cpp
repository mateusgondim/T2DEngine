#include "Texture_2d_manager.hpp"

#include "Resource.hpp"
#include "Resource_manager.hpp"
#include "Texture_2d.hpp"
#include <string>


// CHECK CASES WHERE LOADING FAILS... I.E THE CREATE_OR_RETRIEVE FAIL

namespace gfx {
	Texture_2d_manager g_texture_2d_mgr;
	
	Texture_2d_manager::Texture_2d_manager() : rms::Resource_manager(std::string("texture_2d"), sizeof(Texture_2d), 5, 4) {}

	rms::Resource *Texture_2d_manager::create_impl(const char *name) 
	{
		//get a memory chunck from the pool
		void *ptexture_2d_mem = m_resources_mem_pool.get_element();

		//check if allocation was succefull
		if (ptexture_2d_mem == nullptr) {
			return nullptr;
		}
		else {
			//construct the texture_2d object
			Texture_2d *ptexture_2d_obj = new (ptexture_2d_mem) Texture_2d(this, name);
			return ptexture_2d_obj;
		}
	}

	rms::Resource *Texture_2d_manager::load(const char *name, const char *texture_file_path) 
	{
		std::pair<rms::Resource*, bool> create_or_retrieve_result = create_or_retrieve(name);

		if (create_or_retrieve_result.first != nullptr) {
			//set the texture file path and load the texture
			Texture_2d *ptexture_2d = static_cast<Texture_2d*>(create_or_retrieve_result.first);
			ptexture_2d->set_image_path(texture_file_path);
			ptexture_2d->load();

			return ptexture_2d;
		}
		else {
			return nullptr;
		}
	}
}
