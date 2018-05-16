#ifndef _SYSTEMS_HPP
#define _SYSTEMS_HPP

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

//Game Object Model
#include "Game_object_manager.hpp"

class Tile_map;

class Systems final {
public:
	Systems() = default;
	void init(const uint32_t context_version_major, const uint32_t context_version_minor, Tile_map *ptile_map);
	void shut_down();
	~Systems() {}
public:
	//Engine managers
	Timer						 m_timer;
	physics_2d::Physics_manager  m_physics_manager;
	gfx::Graphics_manager        m_graphics_manager;
	Input_manager				 m_input_manager;
	gom::Game_object_manager     m_game_object_manager;
	
	//Resource managers
	gfx::Shader_manager			m_shader_manager;
	gfx::Texture_2d_manager	    m_texture_manager;
	gfx::Sprite_atlas_manager   m_atlas_manager;

};

extern Systems g_systems;
#endif // !_SUB_SYSTEMS_HPP
#pragma once
