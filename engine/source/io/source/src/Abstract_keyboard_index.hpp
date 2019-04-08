#ifndef _ABSTRACT_KEYBOARD_INDEX_HPP
#define _ABSTRACT_KEYBOARD_INDEX_HPP

namespace io
{
        /**
         * The indexes of the keyboard's keys used by the Engine.
         *
         * The T2DEngine stores the state of the Keyboard's keys/buttons on a
         * 64-bit unsigned variable, with each bit representing a key value, to map
         * each bit to a specific key, we use the indexes defined in this Enum.
         */
        enum Abstract_keyboard_index {
                KEY_A = 0, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K,
                KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W,
                KEY_X, KEY_Y, KEY_Z, KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8,
                KEY_9, KEY_SPACE, KEY_ENTER, KEY_BACKSPACE, KEY_RIGHT, KEY_LEFT, KEY_DOWN, KEY_UP,
                KEY_UNKNOWN
        };
}
#endif // !_ABSTRACT_KEYBOARD_INDEX_HPP
 