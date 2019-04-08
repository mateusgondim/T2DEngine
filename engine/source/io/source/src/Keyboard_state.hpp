#ifndef _KEYBOARD_STATE_HPP
#define _KEYBOARD_STATE_HPP

#include <stdint.h>

namespace io
{
        /**
         * Stores the keyboard's keys that were: pressed this frame, released
         * this frame and, pressed for more than one frame.
         */
        class Keyboard_state {
        public:
                explicit        Keyboard_state(uint64_t buttons_mask = 0U);
                uint64_t        buttons_just_went_down(uint64_t buttons_mask) const;
                uint64_t        buttons_just_went_up(uint64_t buttons_mask) const;
                uint64_t        buttons_down(uint64_t buttons_mask) const;
                void            update_buttons();
        private:
                uint64_t        m_buttons_state;
                uint64_t        m_prev_buttons_state;
                uint64_t        m_buttons_down; // Buttons that just went down this frame
                uint64_t        m_buttons_up;   // Buttons that just went up this frame
        };

        inline uint64_t Keyboard_state::buttons_just_went_down(uint64_t buttons_mask) const
        {
                return m_buttons_down & buttons_mask;
        }

        inline uint64_t Keyboard_state::buttons_just_went_up(uint64_t buttons_mask) const
        {
                return m_buttons_up & buttons_mask;
        }

        inline uint64_t Keyboard_state::buttons_down(uint64_t buttons_mask) const
        {
                return m_buttons_state & buttons_mask;
        }

}
#endif // !_KEYBOARD_STATE_HPP