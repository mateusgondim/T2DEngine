#include "Level_ui_creator.hpp"

#include "Camera_2d.hpp"
#include "Game_object.hpp"
#include "Rect.hpp"
#include "Game_object_manager.hpp"

#include "Sprite_atlas.hpp"

#include "Resource.hpp"

#include "runtime_memory_allocator.hpp"

#include "UI_manager.hpp"
#include "Canvas.hpp"
#include "Widget.hpp"
#include "Pause_text.hpp"

Level_ui_creator::Level_ui_creator(const string_id canvas_atlas_id) :
        m_canvas_atlas_id(canvas_atlas_id) {}


gom::Game_object * Level_ui_creator::create(const math::vec3 & wld_pos)
{
        math::Rect screen_rect = gom::g_game_object_mgr.get_main_camera().get_screen_rect();
        void *pmem = mem::allocate(sizeof(ui::Canvas));
        ui::Canvas *pcanvas = new (pmem) ui::Canvas(screen_rect, m_canvas_atlas_id);

        pmem = mem::allocate(sizeof(ui::Text));
        Pause_text *ptext = new (pmem) Pause_text(*pcanvas);
        ptext->set_obj_to_canvas_translation(screen_rect.center());
        ptext->set_active(false);

        return pcanvas;
}
