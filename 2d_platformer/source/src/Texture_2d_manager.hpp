#ifndef _TEXTURE_2D_MANAGER_HPP
#define _TEXTURE_2D_MANAGER_HPP
#define  MAX_NUM_TEXTURES 16

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "Texture_2d.hpp"

//TODO: CHANGE m_ptexture_buffer to VECTOR[MAX_NUM_TEXTURES] of pointers and use NEW and DELETE
namespace tgs {
	class Texture_2d_manager {
	public:
		static Texture_2d_manager & instance() 
		{
			static Texture_2d_manager * instance = new Texture_2d_manager();
			return *instance;
		}
		std::shared_ptr<const Texture_2d> load_texture(const std::string & image_path);
		//std::shared_ptr<const Texture_2d> delete_texture(std::shared_ptr<const Texture_2d> sptexture)
	private:
		Texture_2d_manager() : m_max_num_textures(MAX_NUM_TEXTURES), m_num_used_textures(0) {}
	
		unsigned		m_num_used_textures;
		unsigned		m_max_num_textures;
		std::vector<std::shared_ptr<Texture_2d>>	m_texture_buffer; // texture pool 
	};
}
#endif // !_TEXTURE_2D_MANAGER_HPP
