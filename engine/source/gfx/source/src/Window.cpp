#include "Window.hpp"
#include <GLFW/glfw3.h>

#include <utility>
namespace gfx
{
        Window::Window(int width, int height, const char * ptitle) : 
                m_width(width), m_height(height), m_title(ptitle)
        {
                m_pglfw_window = glfwCreateWindow(width, height, ptitle, NULL, NULL);
        }


        std::pair<int, int> Window::get_framebuffer_size() const
        {
                int width, height;
                glfwGetFramebufferSize(m_pglfw_window, &width, &height);
                return std::make_pair(width, height);
        }

        bool Window::should_close() const
        {
                return glfwWindowShouldClose(m_pglfw_window);
        }

        void Window::swap_buffers()
        {
                glfwSwapBuffers(m_pglfw_window);
        }

        bool Window::is_initialized() const
        {
                if (m_pglfw_window) {
                        return true;
                }
                else {
                        return false;
                }
        }

        Window::~Window()
        {
                destroy();
        }

        void Window::destroy()
        {
                if (m_pglfw_window) {
                        glfwDestroyWindow(m_pglfw_window);
                        m_pglfw_window = nullptr;
                }
        }

}