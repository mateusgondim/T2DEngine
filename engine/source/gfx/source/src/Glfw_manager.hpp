#ifndef _GLFW_MANAGER_HPP
#define _GLFW_MANAGER_HPP

namespace gfx { class Window; }
namespace gfx
{
        /**
         * A wrapper for the GLFW library
         *
         */
        class Glfw_manager {
                typedef void (*key_callback_ptr) (GLFWwindow *, int, int, int, int);
                typedef void (*error_callback_ptr) (int const char *);
        public:
                static void          init();
                static void          terminate();
                static void          window_hint(int hint, int value);
                static Window *      create_window(int width, int height, const char *ptitle);
                static void          swap_interval(int interval);
                static void          make_context_current(Window * pwindow);
                static void          poll_events();

                static void         set_error_callback(error_callback_ptr pcallback);
                static void         set_key_callback(Window *pwindow, key_callback_ptr pcallback);
        };
}

#endif // !_GLFW_MANAGER_HPP
