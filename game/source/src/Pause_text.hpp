#ifndef _PAUSE_TEXT_HPP
#define _PAUSE_TEXT_HPP

#include "Text.hpp"

namespace ui { class Canvas; }
namespace math { struct Rect; }
class Event;

class Pause_text final : public ui::Text {
public:
        Pause_text(ui::Canvas & parent_canvas, const math::Rect & rect);

        virtual void on_event(Event & event) override;
};
#endif // !_PAUSED_TEXT_HPP
