#include "Input_abstraction_layer.hpp"
#include "Abstract_keyboard_index.hpp"

#include <stdint.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

uint64_t io::Input_abstraction_layer::s_keyboard_buttons_mask = 0;

/**
 * Converts the GLFW's keyboard's key indexes and key actions to the Engine's format and,
 * stores the current keyboard state on it's static data member as a 64-bit unsigned bitmask.
*/
void io::Input_abstraction_layer::keyboard_callback(GLFWwindow * pwindow, int key, int scancode, int action, int mods)
{
        unsigned abstract_button_index = -1;
        if ((key >= GLFW_KEY_A) && (key <= GLFW_KEY_Z)) {
                unsigned amount_to_displace = GLFW_KEY_A - Abstract_keyboard_index::KEY_A;
                // get the button index on the Abstract_keyboard_index
                abstract_button_index = key - amount_to_displace;
        }
        else if ((key >= GLFW_KEY_0) && (key <= GLFW_KEY_9)) {
                unsigned amount_to_displace = GLFW_KEY_0 - Abstract_keyboard_index::KEY_0;
                abstract_button_index = key - amount_to_displace;
        }
        else if ((key >= GLFW_KEY_RIGHT) && (key <= GLFW_KEY_UP)) {
                unsigned amount_to_displace = GLFW_KEY_RIGHT - Abstract_keyboard_index::KEY_RIGHT;
                abstract_button_index = key - amount_to_displace;
        }
        else {
                switch (key) {
                case GLFW_KEY_SPACE:
                        abstract_button_index = Abstract_keyboard_index::KEY_SPACE;
                        break;
                case GLFW_KEY_ENTER:
                        abstract_button_index = Abstract_keyboard_index::KEY_ENTER;
                        break;
                case GLFW_KEY_BACKSPACE:
                        abstract_button_index = Abstract_keyboard_index::KEY_BACKSPACE;
                        break;
                case GLFW_KEY_UNKNOWN:
                        abstract_button_index = Abstract_keyboard_index::KEY_UNKNOWN;
                }
        }
        unsigned abstract_button_action = (action == GLFW_REPEAT) ? (1) : (action);

        // add to keyboard buttons mask
        uint64_t bit_mask = 1ULL << abstract_button_index;
        if (abstract_button_action == 1) { // turn bit ON
                s_keyboard_buttons_mask |= bit_mask;
        }
        else { //turn bit OFF
                s_keyboard_buttons_mask &= ~bit_mask;
        }
}
