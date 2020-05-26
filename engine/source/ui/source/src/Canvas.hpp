#ifndef _CANVAS_HPP
#define _CANVAS_HPP

#include "Game_object.hpp"
#include "Game_object_handle.hpp"
#include "Rect.hpp"
#include "Sprite_batch.hpp"
#include "string_id.hpp"
#include <cstdint>
#include <string>

namespace ui { class Widget; class Text; class UI_manager; }
namespace gfx { class Sprite_atlas; }
class Event;

namespace ui
{
        class Canvas : public gom::Game_object {
                friend class UI_manager;
        public:
                Widget * create_widget(const math::Rect & rect);
                Text * create_text(const math::Rect & rect, const std::string & msg,
                                   const float scale_factor = 1.0f);

                virtual void                 update(float dt) override;
                virtual void                 on_event(Event & event) override;
                                             ~Canvas();
                gfx::Sprite_atlas *    get_atlas();

        private:
                explicit                     Canvas(const math::Rect & rect,
                                                    const string_id atlas_id);
                void                         render();
                // Maybe the canvas should have a shader program
                static const std::uint8_t    s_max_num_widgets = 10;
                static const std::uint8_t    s_max_num_vertices_per_widget = 48; 
                std::uint8_t                 m_num_widgets;
                bool                         m_dirty;
                string_id                    m_atlas_id;
                math::Rect                   m_rect;
                gfx::Sprite_batch            m_vertex_batch;
                gom::Game_object_handle      m_widgets[s_max_num_widgets];
        };
}

#endif // !_CANVAS_HPP
