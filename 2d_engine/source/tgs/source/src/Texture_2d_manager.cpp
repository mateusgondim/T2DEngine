#include "Texture_2d_manager.hpp"
#include "Texture_2d.hpp"
#include <memory>


std::shared_ptr<const tgs::Texture_2d> tgs::Texture_2d_manager::load_texture(const std::string & image_path) 
{
	for (auto b = m_texture_buffer.cbegin(), e = m_texture_buffer.cend(); b != e; ++b) {
		if ( (*b) -> get_image_path() == image_path ) {
			return *b;	//texture is already on the pool
		}
	}
	// texture is not yet loaded
	if (m_num_used_textures < m_max_num_textures ) {
		m_texture_buffer.push_back(std::make_shared<Texture_2d>(image_path));
		++m_num_used_textures;
		return m_texture_buffer[m_num_used_textures - 1];
	}
}