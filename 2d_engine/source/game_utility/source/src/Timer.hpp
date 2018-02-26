#ifndef _TIMER_HPP
#define _TIMER_HPP

//The global clock of the engine, every system that needs to acess the time, should do it through this class

#define DEFAULT_MS_PER_UPDATE 1/60.0F

class Timer{
public:
	Timer() : m_last_time(0.0f), m_current_time(0.0f), m_delta_time(0.0f), m_time(0.0f), m_curr_fps(0), m_fps_cnt(0), m_fps_time(0.0f) {}
	~Timer() = default;
	
	void           init(float ms_per_update = DEFAULT_MS_PER_UPDATE);
	void		   update();
	float		   get_time()     const  { return m_time; }
	float		   get_dt()       const  { return m_delta_time;}
	float          get_fixed_dt() const  { return m_fixed_dt; }
	int            get_fps()  const { return m_curr_fps; }
private:
	float m_fixed_dt;
	float m_current_time;
	float m_last_time;
	float m_delta_time;
	float m_time;    // time measured since first update call
	
	int   m_fps_cnt;
	int   m_curr_fps; // a frames per second counter
	float m_fps_time;
};


#endif // !_TIMER_HPP
