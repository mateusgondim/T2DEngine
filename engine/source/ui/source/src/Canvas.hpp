#ifndef _CANVAS_HPP
#define _CANVAS_HPP

#include "Game_object.hpp"
#include "Rect.hpp"
#include "Sprite_batch.hpp"
#include "string_id.hpp"
#include <cstdint>

namespace ui { class Widget; class UI_manager; }
class Event;

namespace ui
{
        class Canvas : public gom::Game_object {
                friend class UI_manager;
        public:
                Widget * create_widget(const math::Rect & rect);

                virtual void                 update(float dt) override;
                virtual void                 on_event(Event & event) override;
                                             ~Canvas();
        private:
                explicit                     Canvas(const math::Rect & rect,
                                                    const string_id atlas_id);
                void                         render();
                // Maybe the canvas should have the Texture and shader program
                static const std::uint8_t    s_max_num_widgets = 10;
                static const std::uint8_t    s_vertices_per_widget = 6;
                std::uint8_t                 m_num_widgets;
                bool                         m_dirty;
                string_id                    m_atlas_id;
                math::Rect                   m_rect;
                gfx::Sprite_batch            m_vertex_batch;
                Widget *                     m_pwidgets[s_max_num_widgets];
        };
}

#endif // !_CANVAS_HPP
