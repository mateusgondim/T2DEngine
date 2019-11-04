#include "Widget.hpp"
#include "Canvas.hpp"
#include "Event.hpp"
#include "Game_object.hpp"
#include "Vertex1P1C1UV.hpp"
#include "vec4.hpp"
#include "mat4.hpp"
#include "Rect.hpp"

namespace ui
{
        Widget::Widget(Canvas & parent_canvas) : Widget(parent_canvas, math::Rect()) {}

        Widget::Widget(Canvas & parent_canvas, const math::Rect & rect) :
                gom::Game_object(sizeof(Widget), math::g_zero_vec3),
                m_pparent_canvas(&parent_canvas), m_rect(rect) {}

        void Widget::update(const float dt) {}

        void Widget::on_event(Event & event) {}

        void Widget::get_view_space_vertices(gfx::Vertex1P1C1UV *pbuffer) const
        {
                math::vec4  bottom_left_pos(m_rect.min());
                math::vec4  bottom_right_pos(bottom_left_pos.x + m_rect.width,
                                                   bottom_left_pos.y);
                math::vec4  top_right_pos(m_rect.max());
                math::vec4  top_left_pos(m_rect.x, m_rect.y);

                const math::mat4 & view_space = m_pparent_canvas->get_transform_component()
                                                                  .get_object_to_world();

                // Transform the vertices 
                bottom_left_pos *= view_space;
                bottom_right_pos *= view_space;
                top_right_pos *= view_space;
                top_left_pos *= view_space;
                
                // set the buffer's data
                math::vec4 widget_color(1.0f);
                pbuffer[0].m_col = widget_color;
                pbuffer[0].m_pos = math::vec3(bottom_left_pos.x, bottom_left_pos.y, 0.0f);
                pbuffer[0].m_uv = math::g_zero_vec2;

                pbuffer[1].m_col = widget_color;
                pbuffer[1].m_pos = math::vec3(bottom_right_pos.x, bottom_right_pos.y, 0.0f);
                pbuffer[1].m_uv = math::g_zero_vec2;

                pbuffer[2].m_col = widget_color;
                pbuffer[2].m_pos = math::vec3(top_right_pos.x, top_right_pos.y, 0.0f);
                pbuffer[2].m_uv = math::g_zero_vec2;

                pbuffer[3].m_col = widget_color;
                pbuffer[3].m_pos = math::vec3(top_right_pos.x, top_right_pos.y, 0.0f);
                pbuffer[3].m_uv = math::g_zero_vec2;

                pbuffer[4].m_col = widget_color;
                pbuffer[4].m_pos = math::vec3(top_left_pos.x, top_left_pos.y, 0.0f);
                pbuffer[4].m_uv = math::g_zero_vec2;

                pbuffer[5].m_col = widget_color;
                pbuffer[5].m_pos = math::vec3(bottom_left_pos.x, bottom_left_pos.y, 0.0f);
                pbuffer[5].m_uv = math::g_zero_vec2;
        }

}