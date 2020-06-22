#include "UI_manager.hpp"
#include "Canvas.hpp"
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
    const std::size_t        g_VERTEX_BUFFER_SZ = 156;
    gfx::Vertex1P1C1UV       g_vertex_buffer[g_VERTEX_BUFFER_SZ];

    UI_manager g_ui_mgr;

    const std::uint8_t UI_manager::s_MAX_NUM_CANVASES;

    void UI_manager::init()
    {
        m_num_canvases = 0;
    }

    bool UI_manager::add_canvas(const Canvas & canvas)
    {
        if (m_num_canvases >= s_MAX_NUM_CANVASES) {
            return false;
        }


        m_canvases[m_num_canvases++].bind(canvas);
        return true;
    }


    void UI_manager::set_widgets_shader(gfx::Shader & widgets_shader)
    {
        m_pwidgets_shader = &widgets_shader;
        // TODO: Remove hardcoded value
        m_projection_location = m_pwidgets_shader->get_uniform_location("P");
    }

    void UI_manager::render()
    {
        gom::Camera_2d & level_camera = gom::g_game_object_mgr.get_main_camera();
        m_pwidgets_shader->uniform_matrix4fv(m_projection_location, 1, false,
                                             level_camera.projection().value_ptr());

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
