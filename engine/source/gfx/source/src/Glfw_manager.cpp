#include "Glfw_manager.hpp"
#include "Window.hpp"

#include <GLFW/glfw3.h>

namespace gfx
{
        using Glfw_manager::error_callback_ptr;
        using Glfw_manager::key_callback_ptr;

        static void Glfw_manager::init()
        {
                glfwInit();
        }

        static void Glfw_manager::window_hint(int hint, int value)
        {
                glfwWindowHint(hint, value);
        }

        static Window * Glfw_manager::create_window(int width, int height, const char * ptitle)
        {
                return new Window(width, height, ptitle);
        }

        static void Glfw_manager::swap_interval(int interval)
        {
                glfwSwapInterval(interval);
        }

        static void Glfw_manager::make_context_current(Window * pwindow)
        {
                glfwMakeContextCurrent(pwindow->m_pglfw_window);
        }

        static void Glfw_manager::set_error_callback(error_callback_ptr pcallback)
        {
                glfwSetErrorCallback(pcallback);
        }

        static void Glfw_manager::set_key_callback(Window *pwindow, key_callback_ptr pcallback)
        {
                glfwSetKeyCallback(pwindow->m_pglfw_window, pcallback);
        }

        static void Glfw_manager::poll_events()
        {
                glfwPollEvents();
        }

        static void Glfw_manager::terminate()
        {
                glfwTerminate();
        }
}