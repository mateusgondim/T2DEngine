#ifndef _INPUT_ABSTRACTION_LAYER_HPP
#define _INPUT_ABSTRACTION_LAYER_HPP

#include <stdint.h>
struct GLFWwindow;

namespace io
{
        /** 
         * Allows access to the state of the keyboard's buttons, using the Engine's
         * abstract keyboard indexes.
         *
         * The GLFW input system, sends all the inputs it receives to an assigned function.
         * In the T2DEngine, this function is the Input_abstraction_layer::keyboard_callback,
         * using the parameters received by GLFW, this function converts them to Engine's format and,
         * stores the current keyboard state on its data member.
         */
        class Input_abstraction_layer {
        public:
                static void keyboard_callback(GLFWwindow * pwindow, int key, int scancode, int action, int mods);
                /**
                 * Returns the current state of the keyboard, using the Engine's key indexes
                 */
                static inline uint64_t get_current_keyboard_buttons() { return s_keyboard_buttons_mask; }
        private:
                static uint64_t s_keyboard_buttons_mask; // The bitmask containing the state of the keyboard's keys
        };
}
#endif // !_INPUT_ABSTRACTION_LAYER_HPP

