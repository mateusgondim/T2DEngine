#ifndef _WINDOW_HPP
#define _WINDOW_HPP

#include <utility>
#include <string>

struct GLFWwindow;
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
        public:
                                        Window(int width, int height, const char * ptitle);
                                        Window(Window &) = delete;
                                        Window(Window &&) = delete;
                                        
                Window &                operator=(const Window &) = delete;
                Window &                operator=(Window &&) = delete;

                                        ~Window();

                std::pair<int, int>     get_framebuffer_size() const;
                bool                    should_close() const;
                void                    swap_buffers();
                bool                    is_window_initialized() const;

        private:
                void                    destroy();
                GLFWwindow *            m_pglfw_window;
                int                     m_width;
                int                     m_height;
                std::string             m_title;
        };
}
#endif // !_WINDOW_HPP
