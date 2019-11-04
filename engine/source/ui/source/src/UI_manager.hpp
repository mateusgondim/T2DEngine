#ifndef _UI_MANAGER_HPP
#define _UI_MANAGER_HPP

#include <cstdint>
#include "Rect.hpp"

namespace ui { class Canvas; }
namespace math { struct Rect; }
namespace gfx { class Shader; }

namespace ui
{
        class UI_manager final {
        public:
                UI_manager() = default;
                Canvas *        create_canvas(const math::Rect & rect);
                Canvas *        create_canvas();
                UI_manager(const UI_manager &) = delete;

                UI_manager & operator=(const UI_manager &) = delete;

                void            init();
                void            shut_down();
                void            render();

                void            set_widgets_shader(gfx::Shader & widgets_shader);
        private:
                static const std::uint8_t       s_max_num_canvases = 8;
                std::uint8_t                    m_num_canvases;
                Canvas *                        m_pcanvases[s_max_num_canvases];
                gfx::Shader *                   m_pwidgets_shader;
        };

        extern UI_manager g_ui_mgr;
}
#endif // !_UI_MANAGER_HPP
