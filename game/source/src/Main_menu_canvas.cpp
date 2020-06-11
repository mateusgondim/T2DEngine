#include "Main_menu_canvas.hpp"
#include "Input_manager.hpp"
#include "Level_manager.hpp"
#include "crc32.hpp"

Main_menu_canvas::Main_menu_canvas(const math::Rect & bounds, const string_id atlas_id) :
    ui::Canvas(bounds, atlas_id, sizeof(Main_menu_canvas)) {}

void Main_menu_canvas::update(float dt)
{
    bool start_game_pressed = io::g_input_mgr.get_button_down(SID('action'));
    if (start_game_pressed) {
        level_management::g_level_mgr.request_next_level();
    }
}