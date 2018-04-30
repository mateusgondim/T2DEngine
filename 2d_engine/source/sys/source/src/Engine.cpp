#include "Engine.hpp"

#include "Tile_map.hpp"

Engine g_engine;
void Engine::init(const uint32_t context_version_major, const uint32_t context_version_minor, Tile_map *ptile_map)
{
	m_graphics_manager.init(context_version_major, context_version_minor);
	m_physics_manager.init(ptile_map);
	//m_input_manager.init();
	m_timer.init();
}


void Engine::shut_down() 
{
	g_engine.m_physics_manager.shut_down();
	g_engine.m_graphics_manager.shut_down();
}
