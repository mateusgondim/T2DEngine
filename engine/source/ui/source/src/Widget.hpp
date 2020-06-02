#ifndef _WIDGET_HPP

#define _WIDGET_HPP

#include "Game_object.hpp"
#include "Rect.hpp"
#include "Vertex1P1C1UV.hpp"
#include <cstddef>
#include <utility>

class Event;
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
        protected:
                virtual vertex_data     get_view_space_vertices() const;
                // Button * pbutton_component;
                Canvas *         m_pparent_canvas;
                math::Rect       m_obj_space_aabb;
        };
}
#endif // !_WIDGET_HPP
