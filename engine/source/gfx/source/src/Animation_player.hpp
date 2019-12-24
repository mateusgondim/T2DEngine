#ifndef _ANIMATION_PLAYER_HPP
#define _ANIMATION_PLAYER_HPP

#include <utility>
#include "Animation.hpp"

/*Animation_player: class that stores a animation and,
 * plays the current  animation using the animation's frames_per_second attribute
 */

//TODO: implement SWITCH_ANIMATION
namespace gfx {
	class Animation_player {
	public:
		Animation_player() : m_playing(false) {}
		explicit Animation_player(const Animation & animation);
		void update(const float dt);
		void pause();
		void resume();
		void start_from_beg();
		unsigned get_frame() const { return (m_current_frame < 0) ?(m_animation.get_frame(0)) :(m_animation.get_frame(m_current_frame) ); } //m_current_frame = -1 before first update
		bool    is_playing() const { return m_playing; }
		bool   changed_frame() const { return m_changed_frame; }
	private:
		Animation						   m_animation;
		Animation::frameids_size_type	   m_current_frame;
		float							   m_seconds_per_frame;
		float							   m_next_frame_time;
		float                              m_local_clock;
		bool							   m_playing;
		bool							   m_changed_frame;
	};
}

#endif // !_ANIMATION_PLAYER_HPP
