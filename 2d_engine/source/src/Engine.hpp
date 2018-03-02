#ifndef _ENGINE_HPP
#define _ENGINE_HPP

//utility
#include "Timer.hpp"
//physics
#include "Physics_manager.hpp"
//input
#include "Input_manager.hpp"



class Engine final {
public:
	Engine() = default;
	void init();
	//void shut_down();
	~Engine() {}
public:
	Timer m_timer;
	physics_2d::Physics_manager  m_physics_manager;
	//graphics_manager m_graphics_manager
	Input_manager    m_input_manager;
};

extern Engine g_engine;
#endif // !_ENGINE_HPP
