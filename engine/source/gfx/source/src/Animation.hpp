#ifndef _ANIMATION_HPP
#define _ANIMATION_HPP

#include <vector>
#include "string_id.hpp"
/*Animation: class that stores a vector of indices that represent frames of a animation in 
 * a sprite atlas, the animation can be looped and has a frames per second attribute
 */
namespace gfx {
	class Animation {
	public:
            typedef std::vector<string_id> frameids_vec;
            typedef std::vector<string_id>::size_type frameids_size_type;


		Animation() : m_frame_count(0) {}
		Animation(const std::vector<unsigned> & frames, const float frames_per_second, const bool loop = true) : 
			m_frames(frames), m_frames_per_second(frames_per_second), m_loop(loop), m_playback_rate(1.0f)
		{
			m_frame_count = m_frames.size();
		}

		float					frames_per_second()        const { return m_frames_per_second; }
		frameids_size_type      num_frames()			   const { return m_frame_count; }
		bool				    loop()			           const { return m_loop; }
		string_id				get_frame(const frameids_size_type index)  const { return m_frames[index]; }
		float				    duration()                 const { return m_frames_per_second * m_frame_count; }
		float                   get_playback_rate()        const { return m_playback_rate; }
		void                    set_playback_rate(const float r) { m_playback_rate = r; }
	private:
		bool				    m_loop;
		float				    m_frames_per_second;
		float                   m_playback_rate;
        frameids_size_type      m_frame_count;
        frameids_vec			m_frames;
	};
}

#endif // !_ANIMATION_HPP
