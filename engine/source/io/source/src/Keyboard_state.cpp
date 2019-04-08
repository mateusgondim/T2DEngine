#include "Keyboard_state.hpp"
#include "Input_abstraction_layer.hpp"

#include <stdint.h>

namespace io
{
        Keyboard_state::Keyboard_state(uint64_t buttons_mask) : 
                m_buttons_state(buttons_mask), m_prev_buttons_state(buttons_mask)
        {
                m_buttons_down = m_buttons_state;
                m_buttons_up = ~ m_buttons_down;
        }

        /**
         * Updates the buttons that were: pressed this frame, released this frame and,
         * pressed for more than one frame.
         *
         * This function should be called once per frame, through the input manager, to  
         * to update the state of the keboard's buttons that were pressed and released this  
         * frame. To get the current state of the keyboard, necessary to update it's data members, it
         * interfaces with the Input_abstraction_layer, which stores the state of the keyboard.
         */
        void Keyboard_state::update_buttons()
        {
                // Set up m_prev and m_buttons state
                m_prev_buttons_state = m_buttons_state;
                m_buttons_state = Input_abstraction_layer::get_current_keyboard_buttons();

                // determine if the bit has changed
                uint64_t changed_buttons = m_buttons_state ^ m_prev_buttons_state;

                // set up buttons_down and buttons_up
                m_buttons_down = changed_buttons & m_buttons_state;
                m_buttons_up = changed_buttons & (~m_buttons_state);
        }

}