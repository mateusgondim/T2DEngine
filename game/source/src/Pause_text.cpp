#include "Pause_text.hpp"
#include "Text.hpp"
#include "Canvas.hpp"
#include "Event.hpp"
#include "Level_manager.hpp"

#include "crc32.hpp"

Pause_text::Pause_text(ui::Canvas & parent_canvas, const math::Rect & rect) :
        ui::Text(parent_canvas, rect, "PAUSED", sizeof(Pause_text), 0.5f) {}

void Pause_text::on_event(Event & event)
{
        switch (event.get_type()) {
        case SID('GAME_PAUSED'):
                set_active(true);
                m_pparent_canvas->set_dirty_flag(true);
                break;
        case SID('GAME_RUNNING'):
                set_active(false);
                m_pparent_canvas->set_dirty_flag(true);
                break;
        default:
                break;

        }
}
