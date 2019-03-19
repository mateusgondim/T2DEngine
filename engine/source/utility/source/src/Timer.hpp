#ifndef _TIMER_HPP
#define _TIMER_HPP
#include <stdint.h>

class Timer{
public:
        explicit Timer(float start_time_seconds = 0.0) : m_time_cycles(seconds_to_cycles(start_time_seconds)), m_time_scale(1.0f), m_is_paused(false) {}
        ~Timer() = default;
	
    static void    init();
    void           update(float dt_real_seconds);
    uint64_t       get_time_cycles() const { return m_time_cycles; }
    bool           is_paused() const { return m_is_paused; }
    void           set_paused(bool want_paused) { m_is_paused = want_paused; }
    float          get_time_scale() const { return m_time_scale; }
    void           set_time_scale(float scale) { m_time_scale = scale; }
    static inline  float   get_fixed_delta_time_seconds() { return s_fixed_delta_time_seconds; }

    static inline  uint64_t seconds_to_cycles(float time_seconds)
    {
            return static_cast<uint64_t>(time_seconds * s_cycles_per_second);
    }

    static inline float cycles_to_seconds(uint64_t time_cycles)
    {
            return static_cast<float>(time_cycles) / s_cycles_per_second;
    }
    int            get_fps()  const { return m_curr_fps; }
private:
	
        uint64_t    m_time_cycles;
        float       m_time_scale;
        bool        m_is_paused;

    static      float s_fixed_delta_time_seconds;
    static      float s_cycles_per_second;
	
	int   m_fps_cnt;
	int   m_curr_fps; // a frames per second counter
	float m_fps_time;
};


#endif // !_TIMER_HPP
