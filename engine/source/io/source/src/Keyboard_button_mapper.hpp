#ifndef _KEYBOARD_BUTTON_MAPPER_HPP
#define _KEYBOARD_BUTTON_MAPPER_HPP

#include <map>
#include <stdint.h>

namespace io
{
        /**
         * Converts user-defined indexes to the Engine's abstract keyboard indexes.
         *
         * This class allows the user to associate different indexes/values to 
         * specific keyboard's key indexes, such as 2 -> KEY_A, working bassically as a
         * control scheme.
         */
        class Keyboard_button_mapper {
        public:
                Keyboard_button_mapper() = default;
                explicit Keyboard_button_mapper(const std::map<uint32_t, uint32_t> & actions_to_buttons) 
                        : m_action_to_button(actions_to_buttons) {}

                bool map_action_to_button(uint32_t action, uint32_t button) 
                { 
                        std::map<uint32_t, uint32_t>::const_iterator it = m_action_to_button.find(action);
                        if (it == m_action_to_button.cend()) {
                                m_action_to_button[action] = button;
                                return true;
                        }
                        else {
                                false;
                        }
                }

                // Converts a user defined index to the associated keyboard's key bitmask
                uint64_t get_button_mask_from_action(uint32_t action) const 
                { 
                        std::map<uint32_t, uint32_t>::const_iterator it = m_action_to_button.find(action);
                        if (it != m_action_to_button.cend()) {
                                return 1ULL << (it->second);
                        }
                        else {
                                return -1;
                        }
                }
        private:
                std::map<uint32_t, uint32_t> m_action_to_button;
        };
}

#endif // !_KEYBOARD_BUTTON_MAPPER_HPP
