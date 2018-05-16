#include "Engine.hpp"

#include "Tile_map.hpp"

void Engine::init(const uint32_t context_version_major, const uint32_t context_version_minor, Tile_map *ptile_map)
{
	m_graphics_manager.init(context_version_major, context_version_minor);
	m_physics_manager.init(ptile_map);
	m_game_object_manager.init(&m_atlas_manager, m_physics_manager.get_world());
	//m_input_manager.init();
	m_timer.init();
}


void Engine::shut_down() 
{
	m_game_object_manager.shut_down();
	m_physics_manager.shut_down();
	m_graphics_manager.shut_down();

	m_atlas_manager.unload_all();
	m_texture_manager.unload_all();
	m_shader_manager.unload_all();
}
