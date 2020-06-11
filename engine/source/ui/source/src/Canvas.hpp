#ifndef _CANVAS_HPP
#define _CANVAS_HPP

#include "Game_object.hpp"
#include "Game_object_handle.hpp"
#include "Rect.hpp"
#include "Sprite_batch.hpp"
#include "string_id.hpp"
#include <cstdint>

namespace ui { class Widget; class Text; class UI_manager; }
namespace gfx { class Sprite_atlas; }
class Event;

namespace ui
{
    class Canvas : public gom::Game_object {
        friend class UI_manager;
    public:
        Canvas(const math::Rect & rect, const string_id atlas_id,
               const std::size_t obj_size = sizeof(Canvas));
        Canvas(const Canvas &) = delete;
        ~Canvas();

        Canvas & operator=(const Canvas &) = delete;
        bool add_widget(const Widget & widget);

        void    set_dirty_flag(const bool is_dirty) { m_dirty = is_dirty; }
        virtual void    update(float dt) override;
        virtual void    on_event(Event & event) override;

        bool is_dirty() const { return m_dirty; }
        gfx::Sprite_atlas *    get_atlas();

    private:
        void                         render();
        static const std::uint8_t    s_MAX_NUM_WIDGETS = 10;
        static const std::uint8_t    s_MAX_NUM_VERTICES_PER_WIDGET = 48;
        std::uint8_t                 m_num_widgets;
        bool                         m_dirty;
        string_id                    m_atlas_id;
        math::Rect                   m_rect;
        gfx::Sprite_batch            m_vertex_batch;
        gom::Game_object_handle      m_widgets[s_MAX_NUM_WIDGETS];
    };
}

#endif // !_CANVAS_HPP
