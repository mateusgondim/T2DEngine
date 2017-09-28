#ifndef _ANIMATION_PLAYER_HPP
#define _ANIMATION_PLAYER_HPP

#include <vector>
#include "Animation.hpp"

/*Animation_player: class that stores a vector of animations and,
 * plays the current selected animation using the animation's frames_per_second attribute
 */

//TODO: implement SWITCH_ANIMATION
namespace tgs {
	class Animation_player {
	public:
		Animation_player() : m_playing(false) {}
		explicit Animation_player(const std::vector<Animation> & animations);
		void add_animation(const Animation & animation);
		void update();
		void pause();
		void resume();
		void start_from_beg();
		unsigned get_frame() const { return (m_current_frame < 0) ?(m_animations[m_current_animation].get_frame(0)) :(m_animations[m_current_animation].get_frame(m_current_frame) ); } //m_current_frame = -1 before first update
		bool    is_playing() const { return m_playing; }
	private:
		std::vector<Animation>			   m_animations;
		std::vector<Animation>::size_type  m_current_animation;
		int								   m_current_frame;
		float							   m_seconds_per_frame;
		float							   m_next_frame_time;
		bool							   m_playing;
	};
}

#endif // !_ANIMATION_PLAYER_HPP
