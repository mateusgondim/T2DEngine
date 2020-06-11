#include "Canvas.hpp"
#include "Game_object.hpp"
#include "Widget.hpp"
#include "Text.hpp"
#include "Vertex1P1C1UV.hpp"
#include "Sprite_batch.hpp"
#include "Game_object_handle.hpp"
#include "Game_object_manager.hpp"
#include "Event.hpp"
#include "string_id.hpp"
#include "Resource.hpp"
#include "Sprite_atlas.hpp"
#include "Sprite_atlas_manager.hpp"
#include "UI_manager.hpp"

#include "runtime_memory_allocator.hpp"

#include "vec3.hpp"
#include "Rect.hpp"
#include <cstdint>
#include <utility>


namespace ui
{
    const std::uint8_t    Canvas::s_MAX_NUM_WIDGETS;
    const std::uint8_t    Canvas::s_MAX_NUM_VERTICES_PER_WIDGET;


    Canvas::Canvas(const math::Rect & rect, const string_id atlas_id, const std::size_t obj_size) :
        gom::Game_object(obj_size, math::g_zero_vec3), m_num_widgets(0), m_dirty(true),
        m_atlas_id(atlas_id), m_rect(rect),
        m_vertex_batch(s_MAX_NUM_VERTICES_PER_WIDGET * s_MAX_NUM_WIDGETS, true)
    {
        if (!g_ui_mgr.add_canvas(*this)) {
#ifndef NDEBUG
            std::cerr << "ERROR: Unable to add canvas to UI_MANAGER" << std::endl;
#endif // !NDEBUG
        }
    }

        void Canvas::update(const float dt) {}
        void Canvas::on_event(Event & event) {}

        Canvas * Canvas::add_widget(const Widget & widget)
        {
                if (m_num_widgets >= s_max_num_widgets) {
                        return nullptr;
                }

                m_widgets[m_num_widgets++].bind(widget);
                return this;
        }

        gfx::Sprite_atlas * Canvas::get_atlas()
        {
                rms::Resource * pres = gfx::g_sprite_atlas_mgr.get_by_id(m_atlas_id);
                if (pres) {
                        return static_cast<gfx::Sprite_atlas*>(pres);
                }
                return nullptr;
        }

        void Canvas::render()
        {
                if (m_dirty) {
                        // Clear the vertex buffer
                        m_vertex_batch.reset();

                        // recalculate the widgets vertices
                        std::uint8_t i = 0;
                        Widget::vertex_data rendering_data;
                        gom::Game_object *pgame_object = nullptr;
                        Widget *pwidget = nullptr;
                        for (i; i != m_num_widgets; ++i) {
                                pgame_object = gom::g_game_object_mgr.get_by_handle(m_widgets[i]);
                                // We need to find a way to remove this handle if the object was destroyed!
                                // Perhaps the best solution is indeed to use a std::vector
                                if (!pgame_object) {
                                        continue;
                                }
                                if (!pgame_object->is_active()) {
                                        continue;
                                }

                                pwidget = static_cast<Widget*>(pgame_object);
                                rendering_data = pwidget->get_view_space_vertices();
                                if (rendering_data.first == nullptr || rendering_data.second == 0) {
                                        continue;
                                }

                                m_vertex_batch.add(rendering_data.first, rendering_data.second);
                        }
                        m_dirty = false;
                }
                m_vertex_batch.render();
        }

        Canvas::~Canvas()
        {
                for (std::uint8_t i = 0; i != m_num_widgets; ++i) {
                        gom::g_game_object_mgr.request_destruction(m_widgets[i]);
                }
        }
}