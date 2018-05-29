#include "Engine.hpp"

#include "Timer.hpp"

#include "Shader_manager.hpp"
#include "Texture_2d_manager.hpp"
#include "Sprite_atlas_manager.hpp"

#include "Graphics_manager.hpp"
#include "Physics_manager.hpp"
#include "Game_object_manager.hpp"



void engine_init(const uint32_t context_version_major, const uint32_t context_version_minor, Tile_map *ptile_map)
{
	//Shader_manager, texture_2d_manager and Sprite_atlas_manager dont need to be explicitly initialized

	//initialize the other engine global managers
	gfx::g_graphics_mgr.init(context_version_major, context_version_minor);
	physics_2d::g_physics_mgr.init(ptile_map);
	gom::g_game_object_mgr.init();
}


void engine_shut_down() 
{
	gom::g_game_object_mgr.shut_down();
	physics_2d::g_physics_mgr.shut_down();
	gfx::g_graphics_mgr.shut_down();

	gfx::g_sprite_atlas_mgr.unload_all();
	gfx::g_texture_2d_mgr.unload_all();
	gfx::g_shader_mgr.unload_all();
}
