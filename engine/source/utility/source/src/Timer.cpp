#include "Timer.hpp"

 #define GLEW_STATIC
 #include <GL/glew.h>
 #include <GLFW/glfw3.h>



float Timer::s_fixed_delta_time_seconds;
float Timer::s_cycles_per_second;

void Timer::init()
{
        s_cycles_per_second = static_cast<float>(glfwGetTimerFrequency());
        s_fixed_delta_time_seconds = 1.0f / 60.0f;
}


void Timer::update(float dt_real_seconds) 
{
        if (!m_is_paused) {
                uint64_t dt_scaled_cycles = seconds_to_cycles(dt_real_seconds * m_time_scale);
                m_time_cycles += dt_scaled_cycles;
        }
        
	// m_current_time = glfwGetTime();
	// m_delta_time = m_current_time - m_last_time;
	// m_last_time = m_current_time;
	// // the overall time elapsed since init
	// m_time			+=   m_delta_time;
// #ifndef NDEBUG
// 	m_fps_time += m_delta_time;
// 	if (m_fps_time > 0.9999f) {
// 		m_curr_fps = m_fps_cnt;
// 		m_fps_time -= 0.9999f;
// 		m_fps_cnt = 1;
// 	}
// 	else {
// 		++m_fps_cnt;
// 	}
// #endif // !NDEBUG

}