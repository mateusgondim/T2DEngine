#include "Canvas.hpp"
#include "Game_object.hpp"
#include "Widget.hpp"
#include "Vertex1P1C1UV.hpp"
#include "Sprite_batch.hpp"
#include "Game_object_handle.hpp"
#include "Game_object_manager.hpp"
#include "Event.hpp"

#include "runtime_memory_allocator.hpp"

#include "vec3.hpp"
#include "Rect.hpp"
#include <cstdint>


namespace ui
{
        const std::uint8_t    Canvas::s_max_num_widgets;
        const std::uint8_t    Canvas::s_vertices_per_widget;


        Canvas::Canvas(const math::Rect & rect) :
                gom::Game_object(sizeof(Canvas), math::g_zero_vec3), m_num_widgets(0),
                m_dirty(true), m_rect(rect),
                m_vertex_batch(s_vertices_per_widget * s_max_num_widgets, true) {}

        void Canvas::update(const float dt) {}
        void Canvas::on_event(Event & event) {}


        Widget * Canvas::create_widget(const math::Rect & rect)
        {
                if (m_num_widgets >= s_max_num_widgets) {
                        return nullptr;
                }

                void *pmem = mem::allocate(sizeof(Widget));
                Widget *pwidget = new (pmem) Widget(*this, rect);

                m_pwidgets[m_num_widgets++] = pwidget;
                m_dirty = true;
                
                return pwidget;
        }

        void Canvas::render()
        {
                if (m_dirty) {
                        // Clear the vertex buffer
                        m_vertex_batch.reset();

                        // recalculate the widgets vertices
                        gfx::Vertex1P1C1UV vertices_buffer[s_vertices_per_widget];
                        std::uint8_t index = 0;
                        for (index; index != m_num_widgets; ++index) {
                                m_pwidgets[index]->get_view_space_vertices(vertices_buffer);
                                m_vertex_batch.add(vertices_buffer, s_vertices_per_widget);
                        }
                        m_dirty = false;
                }
                m_vertex_batch.render();
        }

        Canvas::~Canvas()
        {
                for (std::uint8_t i = 0; i != m_num_widgets; ++i) {
                        gom::Game_object_handle handle(m_pwidgets[i]->get_unique_id(),
                                                       m_pwidgets[i]->get_handle_index());
                        gom::g_game_object_mgr.request_destruction(handle);
                }
        }
}