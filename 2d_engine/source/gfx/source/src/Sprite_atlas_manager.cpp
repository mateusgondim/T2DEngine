#include "Sprite_atlas_manager.hpp"
#include "Resource.hpp"
#include "Resource_manager.hpp"
#include "Sprite_atlas.hpp"
#include "Texture_2d_manager.hpp"
#include "Texture_2d.hpp"

#include <string>
#include <iostream>

namespace gfx {
	Sprite_atlas_manager::Sprite_atlas_manager() : rms::Resource_manager(std::string("sprite_atlas"), sizeof(Sprite_atlas), 5, 4) {}

	rms::Resource* Sprite_atlas_manager::create_impl(const char *name) 
	{
		//get a memory chunck from the pool
		void *psprite_atlas_mem = m_resources_mem_pool.get_element();

		//check if the allocation was successfull
		if (psprite_atlas_mem == nullptr) {
			return nullptr;
		}
		else {
			Sprite_atlas *psprite_atlas_obj = new (psprite_atlas_mem) Sprite_atlas(this, name);
			return psprite_atlas_obj;
		}
	}

	rms::Resource* Sprite_atlas_manager::load(const char *name, const char *file_path, gfx::Texture_2d_manager *texture_manager)
	{
		//create a new Sprite_atlas resource or, if it was already created, retrieve it
		std::pair<rms::Resource*, bool> create_or_retrieve_result = create_or_retrieve(name);

		if (create_or_retrieve_result.first != nullptr) {
			//set the atlas resource file, to be able to load it
			Sprite_atlas *psprite_atlas = static_cast<Sprite_atlas*>(create_or_retrieve_result.first);
			psprite_atlas->set_atlas_file_path(file_path);

			//load the sprite_atlas
			psprite_atlas->load();

			//every sprite atlas resource, is connected to a texture_2d resource, we need to load it was well

			//get the name of the sprite_atlas resource
			std::string texture_name = psprite_atlas->get_texture_file_path();
			auto pos = texture_name.find_last_of('/');
			pos = (pos == std::string::npos) ? (texture_name.find_last_of('\\')) : (pos);
			++pos;
			texture_name = texture_name.substr(pos);

			//load the sprite_atlas resource
			rms::Resource *presource = texture_manager->load(texture_name.c_str(), psprite_atlas->get_texture_file_path());

			if (presource != nullptr) {
				//associate the texture with the corresponding atlas
				psprite_atlas->m_ptexture = static_cast<Texture_2d*>(presource);
				return psprite_atlas;
			}
			else {
				std::cerr << "ERROR(" << __FUNCTION__ << "): Unable to create Texture resource, unloading Sprite_atlas associated with this Texture..." << std::endl;
				unload(psprite_atlas->get_id());
				return nullptr;
			}
		}
		else {
			return nullptr;
		}
	}

	void gfx::Sprite_atlas_manager::unload(string_id id) 
	{
		rms::Resource *presource = get_by_id(id);
		//request the Texture manager to unload the texture resource, associated with this sprite_atlas
		if (presource != nullptr) {
			gfx::Sprite_atlas *psprite_atlas = static_cast<gfx::Sprite_atlas*>(presource);
			psprite_atlas->get_texture()->get_creator()->unload(psprite_atlas->get_texture()->get_id());
			//ptexture_manager->unload(psprite_atlas->get_texture()->get_id());
			
			//unload atlas and remove it from the map
			//deallocate resource
			presource->unload();

			//Remove from the resource map
			m_resources.erase(id);

			//return memory to pooll
			m_resources_mem_pool.free_element(static_cast<void*>(presource));
		}
	}

	void gfx::Sprite_atlas_manager::unload(const char *name)
	{
		rms::Resource *presource = get_by_name(name);
		//request the Texture manager to unload the texture resource, associated with this sprite_atlas
		if (presource != nullptr) {
			gfx::Sprite_atlas *psprite_atlas = static_cast<gfx::Sprite_atlas*>(presource);
			psprite_atlas->get_texture()->get_creator()->unload(psprite_atlas->get_texture()->get_id());
			//ptexture_manager->unload(psprite_atlas->get_texture()->get_id());
			
			string_id id = presource->get_id();
			//deallocate resource
			presource->unload();

			//Remove from the resource map
			m_resources.erase(id);

			//return memory to pooll
			m_resources_mem_pool.free_element(static_cast<void*>(presource));
		}
	}

	void gfx::Sprite_atlas_manager::unload_all() 
	{
		resource_map::iterator it;

		for (it = m_resources.begin(); it != m_resources.end(); ++it) {
			gfx::Sprite_atlas *psprite_atlas = static_cast<gfx::Sprite_atlas*>(it->second);
			//unload texture resource associated with this sprite_atlas resource
			psprite_atlas->get_texture()->get_creator()->unload(psprite_atlas->get_texture()->get_id());
			
			//unload atlas
			psprite_atlas->unload();

			//give back the resource's memory to the pool
			m_resources_mem_pool.free_element(static_cast<void*>(it->second));
		}
		//clear the map
		m_resources.clear();
	}
}
