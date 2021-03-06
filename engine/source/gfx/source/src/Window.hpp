#ifndef _WINDOW_HPP
#define _WINDOW_HPP

#include <utility>
#include <string>

struct GLFWwindow;
namespace gfx { class Glfw_manager; }
namespace gfx
{
        /**
         * Handles a window with an OpenGL context that can receive input events 
         *
         * This class provides a window with an OpenGL context, required by 
         * the graphics manager to render the level. It also allows the 
         * registering of callbacks that are fired by io events on the window.
         */
        class Window {
                friend class Glfw_manager;
        public:
                                        Window(int width, int height, float scale,
                                               const char * ptitle);
                                        Window(Window &) = delete;
                                        Window(Window &&) = delete;
                                        
                Window &                operator=(const Window &) = delete;
                Window &                operator=(Window &&) = delete;

                                        ~Window();

                void                    update_viewport();
                std::pair<int, int>     get_framebuffer_size() const;
                void                    set_title(const char * ptitle);
                void                    set_size(int width, int height);
                bool                    should_close() const;
                void                    swap_buffers();
                bool                    is_initialized() const;

        private:
                void                    destroy();
                GLFWwindow *            m_pglfw_window;
                int                     m_virtual_width;
                int                     m_virtual_height;
                float                   m_virtual_aspect_ratio;
                float                   m_scale;
                int                     m_width;
                int                     m_height;
                std::string             m_title;
        };
}
#endif // !_WINDOW_HPP
