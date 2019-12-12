#include "Window.hpp"
#include <GLFW/glfw3.h>

#include <utility>
namespace gfx
{
        Window::Window(int width, int height, float scale, const char * ptitle) : 
                m_virtual_width(width), m_virtual_height(height),
                m_virtual_aspect_ratio((float)width / height), m_scale(scale),
                m_width(width * scale), m_height(height * scale), m_title(ptitle)
        {
                m_pglfw_window = glfwCreateWindow(m_width, m_height, ptitle, NULL, NULL);
                glfwSetWindowSizeLimits(m_pglfw_window, m_virtual_width, m_virtual_height,
                                        GLFW_DONT_CARE, GLFW_DONT_CARE);
        }


        std::pair<int, int> Window::get_framebuffer_size() const
        {
                int width, height;
                glfwGetFramebufferSize(m_pglfw_window, &width, &height);
                return std::make_pair(width, height);
        }

        void Window::set_title(const char * ptitle)
        {
                glfwSetWindowTitle(m_pglfw_window, ptitle);
        }

        void Window::set_size(int width, int height)
        {
                glfwSetWindowSize(m_pglfw_window, width, height);
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