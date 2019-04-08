#include "Engine.hpp"

#include "Timer.hpp"
#include "Path.hpp"

#include "Shader_manager.hpp"
#include "Texture_2d_manager.hpp"
#include "Sprite_atlas_manager.hpp"
#include "Input_abstraction_layer.hpp"

#include "Graphics_manager.hpp"
#include "Physics_manager.hpp"
#include "Game_object_manager.hpp"
#include "Projectile_manager.hpp"
#include "Level_manager.hpp"


static void error_callback(int error, const char * descr)
{
	std::cerr << "GLFW ERROR: " << descr << std::endl;
}

void engine_init(const uint32_t context_version_major, const uint32_t context_version_minor, Tile_map *ptile_map)
{
	//Shader_manager, texture_2d_manager and Sprite_atlas_manager dont need to be explicitly initialized

	//initialize the  engine global managers
	gfx::g_graphics_mgr.init(context_version_major, context_version_minor);
	physics_2d::g_physics_mgr.init(ptile_map);
	gom::g_game_object_mgr.init();
	gom::g_projectile_mgr.init();

    // ----------------------------------------New initialization code--------------------------------------------------------------------------------------------
    gfx::g_graphics_mgr.set_error_callback(error_callback);
    gfx::g_graphics_mgr.create_window(512, 480, "2D Game Project");
    gfx::g_graphics_mgr.set_key_callback(io::Input_abstraction_layer::keyboard_callback);
    
    Path resources_path("../resources", Path::FORWARD_SLASH);
    gom::g_level_mgr.load(resources_path, ptile_map);
}


void engine_shut_down() 
{
	gom::g_projectile_mgr.shut_down();
	gom::g_game_object_mgr.shut_down();
	physics_2d::g_physics_mgr.shut_down();
	gfx::g_graphics_mgr.shut_down();

	gfx::g_sprite_atlas_mgr.unload_all();
	gfx::g_texture_2d_mgr.unload_all();
	gfx::g_shader_mgr.unload_all();
}
