#include "UI_manager.hpp"
#include "Rect.hpp"
#include "Canvas.hpp"
#include "runtime_memory_allocator.hpp"
#include "Camera_2d.hpp"
#include "Game_object.hpp"
#include "Game_object_handle.hpp"
#include "Game_object_manager.hpp"
#include "Shader.hpp"
#include "Texture_2d.hpp"
#include "Sprite_atlas.hpp"
#include "mat4.hpp"

#include <cstdint>

namespace ui
{
        const std::size_t        g_vertex_buffer_sz = 156;
        gfx::Vertex1P1C1UV       g_vertex_buffer[g_vertex_buffer_sz];

        UI_manager g_ui_mgr;

        const std::uint8_t UI_manager::s_max_num_canvases;

        void UI_manager::init()
        {
                m_num_canvases = 0;
        }

        Canvas * UI_manager::create_canvas(const math::Rect & rect, gfx::Sprite_atlas & atlas)
        {
                if (m_num_canvases >= s_max_num_canvases) {
                        return nullptr;
                }

                void *pmem = mem::allocate(sizeof(Canvas));
                Canvas *pcanvas = new (pmem) Canvas(rect, atlas.get_id());
                m_canvases[m_num_canvases++].bind(*pcanvas);

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

        Canvas * UI_manager::create_canvas(gfx::Sprite_atlas & atlas)
        {
                return create_canvas(math::Rect(), atlas);
        }

        void UI_manager::render()
        {
                std::int32_t p_location = m_pwidgets_shader->get_uniform_location("P");
                gom::Camera_2d * plevel_camera = gom::g_game_object_mgr.get_main_camera();
                m_pwidgets_shader->uniform_matrix4fv(p_location, 1, false,
                                                     plevel_camera->projection().value_ptr());

                gom::Game_object *pgame_object = nullptr;
                Canvas *pcanvas = nullptr;
                for (std::uint8_t i = 0; i != m_num_canvases; ++i) {
                        pgame_object = gom::g_game_object_mgr.get_by_handle(m_canvases[i]);
                        if (!pgame_object) {
                                continue;
                        }

                        pcanvas = static_cast<Canvas*>(pgame_object);
                        if (pcanvas->is_active()) {
                                gfx::Sprite_atlas * patlas = pcanvas->get_atlas();
                                patlas->get_texture()->use();
                                pcanvas->render();
                        }
                }
        }

        void UI_manager::shut_down()
        {
                for (std::uint8_t i = 0; i != m_num_canvases; ++i) {
                        gom::g_game_object_mgr.request_destruction(m_canvases[i]);
                }
        }

}
