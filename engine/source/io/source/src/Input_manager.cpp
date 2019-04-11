#include "Input_manager.hpp"
#include "Keyboard_state.hpp"
#include "Keyboard_button_mapper.hpp"

#include <stdint.h>

io::Input_manager io::g_input_mgr;

namespace io {

        void Input_manager::pool_events()
        {
                m_keyboard_state.update_buttons();
        }

        bool Input_manager::get_button_down(unsigned action_index) const
        {
                uint64_t button_mask = m_keyboard_control_scheme.get_button_mask_from_action(action_index);
                return m_keyboard_state.buttons_just_went_down(button_mask);
        }

        bool Input_manager::get_button_up(unsigned action_index) const 
        {
                uint64_t button_mask = m_keyboard_control_scheme.get_button_mask_from_action(action_index);
                return m_keyboard_state.buttons_just_went_up(button_mask);
        }

        bool Input_manager::get_button(unsigned action_index) const
        {
                uint64_t button_mask = m_keyboard_control_scheme.get_button_mask_from_action(action_index);
                return m_keyboard_state.buttons_down(button_mask);
        }

}
