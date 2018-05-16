#ifndef _ENGINE_HPP
#define _ENGINE_HPP

#include <stdint.h>

//utility
#include "Timer.hpp"
//physics
#include "Physics_manager.hpp"
//input
#include "Input_manager.hpp"
// graphics
#include "Graphics_manager.hpp"
#include "Shader_manager.hpp"
#include "Texture_2d_manager.hpp"
#include "Sprite_atlas_manager.hpp"

//GOM
#include "Game_object_manager.hpp"

class Tile_map;

//MAYBE CHANGE TO POINTERS TO MANAGER, TO BE ABLE TO DO A FORWARD DECLARATION.. THERE ARE TO MANY HEADERS...

class Engine final {
public:
	Engine() = default;
	void init(const uint32_t context_version_major, const uint32_t context_version_minor, Tile_map *ptile_map);
	void shut_down();
	~Engine() {}
public:
	Timer						 m_timer;
	physics_2d::Physics_manager  m_physics_manager;
	gfx::Graphics_manager        m_graphics_manager;
	Input_manager				 m_input_manager;

	gfx::Shader_manager			m_shader_manager;
	gfx::Texture_2d_manager		m_texture_manager;
	gfx::Sprite_atlas_manager   m_atlas_manager;

	gom::Game_object_manager	m_game_object_manager;
};

#endif // !_ENGINE_HPP
