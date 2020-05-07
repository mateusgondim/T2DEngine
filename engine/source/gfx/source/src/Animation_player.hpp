#ifndef _ANIMATION_PLAYER_HPP
#define _ANIMATION_PLAYER_HPP

#include <utility>
#include "string_id.hpp"
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
        string_id get_frame() const;
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

    inline string_id Animation_player::get_frame() const
    {
            Animation::frameids_size_type undefined_frame = -1;
            if (m_current_frame == undefined_frame) {
                    return m_animation.get_frame(0);
            }
            return m_animation.get_frame(m_current_frame);
    }
}

#endif // !_ANIMATION_PLAYER_HPP
