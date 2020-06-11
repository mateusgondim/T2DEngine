#ifndef _WIDGET_HPP

#define _WIDGET_HPP

#include "Game_object.hpp"
#include "Rect.hpp"
#include "vec2.hpp"
#include <cstddef>
#include <utility>

class Event;
namespace gfx { struct Vertex1P1C1UV; }
namespace ui { class Canvas; }

namespace ui
{
        class Widget : public gom::Game_object {
                friend class Canvas;
        public:
                typedef std::pair<gfx::Vertex1P1C1UV *, std::size_t> vertex_data;

                virtual void            update(const float dt) override;
                virtual void            on_event(Event & event) override;
                explicit Widget(Canvas & parent_canvas);
                         Widget(Canvas & parent_canvas, const math::Rect & obj_space_aabb,
                                const std::size_t obj_sz = sizeof(Widget));
                         bool has_parent() const 
                         {
                                 return (m_pparent_canvas) ? (true) : (false);
                         }

                         math::Rect get_obj_space_rect() const { return m_obj_space_aabb; }
                         void       set_obj_to_canvas_translation(const math::vec2 & t)
                         {
                                 m_obj_to_canvas_translation = t;
                         }
        protected:
                virtual vertex_data     get_view_space_vertices() const;
                // Button * pbutton_component;
                Canvas *         m_pparent_canvas;
                math::Rect       m_obj_space_aabb;
                math::vec2       m_obj_to_canvas_translation;
        };
}
#endif // !_WIDGET_HPP
