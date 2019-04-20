#include "Glfw_manager.hpp"
#include "Window.hpp"

#include <GLFW/glfw3.h>

namespace gfx
{
        void Glfw_manager::init(int context_version_major, int context_version_minor)
        {
                glfwInit();
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, context_version_major);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, context_version_minor);
                // Use a forward-compatible context
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
                // Use OpenGl core profile
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }

        void Glfw_manager::window_hint(int hint, int value)
        {
                glfwWindowHint(hint, value);
        }

        Window * Glfw_manager::create_window(int width, int height, const char * ptitle)
        {
                return new Window(width, height, ptitle);
        }

        void Glfw_manager::swap_interval(int interval)
        {
                glfwSwapInterval(interval);
        }

        void Glfw_manager::make_context_current(Window * pwindow)
        {
                glfwMakeContextCurrent(pwindow->m_pglfw_window);
        }

        void Glfw_manager::set_error_callback(error_callback_ptr pcallback)
        {
                glfwSetErrorCallback(pcallback);
        }

        void Glfw_manager::set_key_callback(Window *pwindow, key_callback_ptr pcallback)
        {
                glfwSetKeyCallback(pwindow->m_pglfw_window, pcallback);
        }

        void Glfw_manager::poll_events()
        {
                glfwPollEvents();
        }

        void Glfw_manager::terminate()
        {
                glfwTerminate();
        }
}