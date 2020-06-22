#ifndef _UI_MANAGER_HPP
#define _UI_MANAGER_HPP

#include <cstdint>
#include "Rect.hpp"
#include "Vertex1P1C1UV.hpp"
#include "Game_object_handle.hpp"

namespace ui { class Canvas; }
namespace math { struct Rect; }
namespace gfx { class Shader; class Sprite_atlas; }

namespace ui
{
        class UI_manager final {
        public:
                UI_manager() = default;
                UI_manager(const UI_manager &) = delete;
                ~UI_manager() = default;

                UI_manager & operator=(const UI_manager &) = delete;

                void    init();
                void    shut_down();
                void    render();
                bool    add_canvas(const Canvas & canvas);

                // Warning: This does not destory the canvas objects
                void            reset() { m_num_canvases = 0; }
                void            set_widgets_shader(gfx::Shader & widgets_shader);
        private:
                static const std::uint8_t       s_MAX_NUM_CANVASES = 8;
                std::uint8_t                    m_num_canvases;
                std::int32_t                    m_projection_location;
                gom::Game_object_handle         m_canvases[s_MAX_NUM_CANVASES];
                gfx::Shader *                   m_pwidgets_shader;
        };

        extern UI_manager g_ui_mgr;

        // this buffer should be temporarily! the idea is to implement a stack allocator
        // and use it for the UI module as a hole
        extern const std::size_t        g_VERTEX_BUFFER_SZ;
        extern gfx::Vertex1P1C1UV       g_vertex_buffer[];
}
#endif // !_UI_MANAGER_HPP
