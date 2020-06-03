#include "Pause_text.hpp"
#include "Text.hpp"
#include "Canvas.hpp"
#include "Event.hpp"
#include "Level_manager.hpp"

#include "crc32.hpp"

Pause_text::Pause_text(ui::Canvas & parent_canvas) :
        ui::Text(parent_canvas, "paused", sizeof(Pause_text), 0.2f, 5) {}

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
