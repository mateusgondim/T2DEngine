#include "Sprite_atlas_manager.hpp"
#include "Sprite_atlas.hpp"
#include <vector>
#include <memory>
#include <string>

std::shared_ptr<const tgs::Sprite_atlas> tgs::Sprite_atlas_manager::load_atlas(const std::string & file_path, const float pixels_per_unit) 
{
	for (auto b = m_atlas_buffer.cbegin(), e = m_atlas_buffer.cend(); b != e; ++b) {
		if ((*b)->get_file_path() == file_path) {
			return *b;	//atlas is already loaded
		}
	}
	// texture is not yet loaded
	if (m_num_used_atlases < m_max_num_atlases) {
		m_atlas_buffer.push_back(std::make_shared<Sprite_atlas>(file_path, pixels_per_unit));
		++m_num_used_atlases;
		return m_atlas_buffer[m_num_used_atlases - 1];
	}
}