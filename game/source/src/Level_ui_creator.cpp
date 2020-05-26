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
        math::Rect screen_rect = gom::g_game_object_mgr.get_main_camera()->get_screen_rect();
        rms::Resource * pres = gfx::g_sprite_atlas_mgr.get_by_id(m_canvas_atlas_id);
        gfx::Sprite_atlas * pui_atlas = static_cast<gfx::Sprite_atlas*>(pres);
        static_cast<gfx::Sprite_atlas*>(pres);
        ui::Canvas *pcanvas = ui::g_ui_mgr.create_canvas(screen_rect, *pui_atlas);

        float width = screen_rect.width / 4.0f;
        float height = screen_rect.height / 6.0f;
        math::Rect widget_rect(screen_rect.center().x - width / 2.0f,
                               screen_rect.center().y + height,
                               width, height);

        void * pmem = mem::allocate(sizeof(ui::Text));
        Pause_text *ptext = new (pmem) Pause_text(*pcanvas, widget_rect);
        ptext->set_active(false);

        return pcanvas;
}
