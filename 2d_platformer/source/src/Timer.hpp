#ifndef _TIMER_HPP
#define _TIMER_HPP

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//The global clock of the engine, every system that needs to acess the time, should do it through this class

class Timer{
public:
	static Timer & instance()
	{
		static Timer * instance = new Timer();
		return *instance;
	}

	void		   update();
	float		   get_time()   const  { return m_time; }
	float		   get_delta()  const  { return m_delta_time;}
private:
	Timer();
	float m_current_time;
	float m_last_time;
	float m_delta_time;
	float m_time;    // time measured since first update call
};


#endif // !_TIMER_HPP
