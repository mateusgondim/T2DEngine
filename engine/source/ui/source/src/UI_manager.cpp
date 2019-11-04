#include "UI_manager.hpp"
#include "Rect.hpp"
#include "Canvas.hpp"
#include "runtime_memory_allocator.hpp"
#include "Camera_2d.hpp"
#include "Game_object.hpp"
#include "Game_object_handle.hpp"
#include "Game_object_manager.hpp"
#include "Shader.hpp"

#include <cstdint>

namespace ui
{
        UI_manager g_ui_mgr;

        const std::uint8_t UI_manager::s_max_num_canvases;

        void UI_manager::init()
        {
                m_num_canvases = 0;
        }

        Canvas * UI_manager::create_canvas(const math::Rect & rect)
        {
                if (m_num_canvases >= s_max_num_canvases) {
                        return nullptr;
                }

                void *pmem = mem::allocate(sizeof(Canvas));
                Canvas *pcanvas = new (pmem) Canvas(rect);
                m_pcanvases[m_num_canvases++] = pcanvas;

                return pcanvas;
        }

        void UI_manager::set_widgets_shader(gfx::Shader & widgets_shader)
        {
                m_pwidgets_shader = &widgets_shader;
        }

        // By the default, the canvas should ocupy the entire screen
        // maybe we should find a way to get the scene camera, and use its left, right,
        // bottom and top values, to initialize the Canvas's rect
        // TODO: set the Rect to be the size of the screen!!!!!

        Canvas * UI_manager::create_canvas()
        {
                return create_canvas(math::Rect());
        }

        void UI_manager::render()
        {
                // m_pwidgets_shader->use();
                
                std::int32_t location = m_pwidgets_shader->get_uniform_location("P");
                gom::Camera_2d * plevel_camera = gom::g_game_object_mgr.get_main_camera();
                m_pwidgets_shader->uniform_matrix4fv(location, 1, false,
                                                     plevel_camera->projection().value_ptr());

                for (std::uint8_t i = 0; i != m_num_canvases; ++i) {
                        if (m_pcanvases[i]->is_active()) {
                                m_pcanvases[i]->render();
                        }
                }
        }

        void UI_manager::shut_down()
        {
                for (std::uint8_t i = 0; i != m_num_canvases; ++i) {
                        gom::Game_object_handle handle(m_pcanvases[i]->get_unique_id(),
                                                       m_pcanvases[i]->get_handle_index());
                        gom::g_game_object_mgr.request_destruction(handle);
                }
        }

}
