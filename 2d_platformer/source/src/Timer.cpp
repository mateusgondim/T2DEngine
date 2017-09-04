#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Timer.hpp"

Timer::Timer() : m_last_time(0.0f), m_current_time(0.0f), m_delta_time(0.0f), m_time(0.0f) {}

void Timer::update() 
{
	m_last_time     =    m_current_time;
	m_current_time  =    glfwGetTime();
	m_delta_time    =    m_current_time - m_last_time;
	m_time			+=   m_delta_time;
}