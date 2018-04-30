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

class Tile_map;

class Systems final {
public:
	Systems() = default;
	void init(const uint32_t context_version_major, const uint32_t context_version_minor, Tile_map *ptile_map);
	void shut_down();
	~Systems() {}
public:
	Timer						 m_timer;
	physics_2d::Physics_manager  m_physics_manager;
	gfx::Graphics_manager        m_graphics_manager;
	Input_manager				 m_input_manager;
};

extern Systems g_systems;
#endif // !_SUB_SYSTEMS_HPP
#pragma once
