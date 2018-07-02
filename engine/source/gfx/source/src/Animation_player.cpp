#include <vector>
#include <iostream>

#include "Animation_player.hpp"
#include "Animation.hpp"


gfx::Animation_player::Animation_player(const gfx::Animation & animation) 
	: m_animation(animation), m_playing(true), m_current_frame(-1), m_local_clock(0.0f), m_changed_frame(true)
{
	m_seconds_per_frame = 1.0f / m_animation.frames_per_second();
	//m_next_frame_time   = g_timer.get_time();
	m_next_frame_time     = m_local_clock;
}

/*
void gfx::Animation_player::add_animation(const Animation & animation)
{
	m_animations.push_back(animation);

	if (m_animations.size() == 1) { //vector was empty
		m_current_animation = 0;
		m_current_frame = -1;
		m_seconds_per_frame = 1.0f / m_animations[m_current_animation].frames_per_second();
	}
}*/

void gfx::Animation_player::update(const float dt)
{	
	if (m_playing) {
		float scaled_dt = dt * m_animation.get_playback_rate();
		m_local_clock += scaled_dt;
	}
	else {
		return;
	}

	if (m_next_frame_time > m_local_clock) { //if it is not yet time to change to the next frame 
		m_changed_frame = false;
		return;
	}
	/*
	if ( (m_next_frame_time > g_timer.get_time()) || (!m_playing) ) { //if it is not yet time to change to the next frame 
		return;
	}*/
	
	++m_current_frame;
	
	if (m_current_frame >= m_animation.num_frames()) { // played the last frame
		if (m_animation.loop()) { // looped animation
			m_current_frame = 0;
		}
		else {
			m_current_frame--;
			m_playing = false;
			return;
		}
	}
	//m_next_frame_time = g_timer.get_time() + m_seconds_per_frame; //calculate when to switch to next frame
	m_next_frame_time   = m_local_clock + m_seconds_per_frame;
	m_changed_frame = true;
	//std::cout << "current frame: " << m_current_frame << "  next frame time: " << m_next_frame_time << std::endl;
}

//pauses the current animation
void gfx::Animation_player::pause()
{
	m_playing = false;
}
//continuies from where it was
void gfx::Animation_player::resume()
{
	if (!m_playing) {
		m_playing = true;
		//m_local_clock = m_next_frame_time = 0.0f;
		//m_next_frame_time = g_timer.get_time();// +m_seconds_per_frame;
		//update();
	}
}

void gfx::Animation_player::start_from_beg()
{
	if (!m_playing) {
		m_changed_frame = true;
		m_playing = true;
		m_current_frame = 0;
		m_local_clock = 0.0f;
		m_next_frame_time = m_seconds_per_frame;
		//m_next_frame_time = g_timer.get_time() + m_seconds_per_frame;
		//update();
	}
}

/*
void gfx::Animation_player::switch_animation(const std::vector<gfx::Animation>::size_type next_anim)
{
	m_current_animation = next_anim;

	m_current_frame = -1;
	m_seconds_per_frame = 1.0f / m_animations[m_current_animation].frames_per_second();
	m_local_clock = m_next_frame_time = 0.0f;
	m_changed_frame = true;
	//m_next_frame_time = g_timer.get_time();
}*/