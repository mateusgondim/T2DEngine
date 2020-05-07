#ifndef _WIDGET_HPP

#define _WIDGET_HPP

#include "Game_object.hpp"
#include "Rect.hpp"
#include "Vertex1P1C1UV.hpp"
#include <cstddef>

class Event;
namespace ui { class Canvas; }

namespace ui
{
        class Widget : public gom::Game_object {
                friend class Canvas;
        public:
                virtual void update(const float dt) override;
                virtual void on_event(Event & event) override;
        private:
                explicit Widget(Canvas & parent_canvas);
                         Widget(Canvas & parent_canvas, const math::Rect & rect,
                                const std::size_t obj_sz = sizeof(Widget));
                // Button * pbutton_component;
                Canvas *         m_pparent_canvas;
                math::Rect       m_rect;
                void             get_view_space_vertices(gfx::Vertex1P1C1UV * pbuffer) const;
        };
}
#endif // !_WIDGET_HPP
