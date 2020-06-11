#ifndef _MAIN_MENU_CANVAS
#define _MAIN_MENU_CANVAS

#include "Canvas.hpp"
#include "string_id.hpp"

namespace math { struct Rect; }

class Main_menu_canvas : public ui::Canvas {
public:
    Main_menu_canvas(const math::Rect & bounds, const string_id atlas_id);

    virtual void update(float dt) override;
};
#endif // !_MAIN_MENU_CANVAS
