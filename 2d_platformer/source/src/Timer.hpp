#ifndef _TIMER_HPP
#define _TIMER_HPP

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//The global clock of the engine, every system that needs to acess the time, should do it through this class

#define DEFAULT_MS_PER_UPDATE 1/60.0F

class Timer{
public:
	Timer() : m_last_time(0.0f), m_current_time(0.0f), m_delta_time(0.0f), m_time(0.0f) {}
	~Timer() = default;
	
	void           init(float ms_per_update = DEFAULT_MS_PER_UPDATE);
	void		   update();
	float		   get_time()     const  { return m_time; }
	float		   get_dt()       const  { return m_delta_time;}
	float          get_fixed_dt() const  { return m_fixed_dt; }
private:
	float m_fixed_dt;
	float m_current_time;
	float m_last_time;
	float m_delta_time;
	float m_time;    // time measured since first update call
};


#endif // !_TIMER_HPP
