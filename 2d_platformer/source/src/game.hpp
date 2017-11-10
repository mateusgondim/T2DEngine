#ifndef _GAME_HPP
#define _GAME_HPP
#include "Timer.hpp"
#include "Physics_manager.hpp"
#include "Input_manager.hpp"

void error_callback(int error, const char * descr);
extern Timer g_timer;
extern physics_2d::Physics_manager g_physics_manager;
extern Input_manager               g_input_manager;
#endif // !_GAME_HPP
