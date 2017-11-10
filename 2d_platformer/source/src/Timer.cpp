#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Timer.hpp"


void Timer::init(float ms_per_update) 
{
	m_fixed_dt = ms_per_update;
	m_last_time = glfwGetTime();
}

void Timer::update() 
{
	m_current_time = glfwGetTime();
	m_delta_time = m_current_time - m_last_time;
	m_last_time = m_current_time;
	// the overall time elapsed since init
	m_time			+=   m_delta_time;
}