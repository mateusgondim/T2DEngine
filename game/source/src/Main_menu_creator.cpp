#include "Main_menu_creator.hpp"

#include "Resource.hpp"
#include "Sprite_atlas.hpp"
#include "vec2.hpp"
#include "Rect.hpp"
#include "Camera_2d.hpp"
#include "Text.hpp"
#include "Main_menu_canvas.hpp"
#include "UI_manager.hpp"

#include "Game_object.hpp"
#include "Game_object_manager.hpp"
#include "runtime_memory_allocator.hpp"


Main_menu_creator::Main_menu_creator(const string_id canvas_atlas_id) :
        m_canvas_atlas_id(canvas_atlas_id) {}


gom::Game_object * Main_menu_creator::create(const math::vec3 & wld_pos)
{
        math::Rect screen_rect = gom::g_game_object_mgr.get_main_camera().get_screen_rect();

        void *pmem = mem::allocate(sizeof(Main_menu_canvas));
        Main_menu_canvas *pcanvas = new (pmem) Main_menu_canvas(screen_rect, m_canvas_atlas_id);

        
        pmem = mem::allocate(sizeof(ui::Text));
        ui::Text *ptitle = new (pmem) ui::Text(*pcanvas, "T2D ENGINE", sizeof(ui::Text), 0.5f, 5);

        pmem = mem::allocate(sizeof(ui::Text));
        ui::Text *psubtitle = new (pmem) ui::Text(*pcanvas, "DEMO", sizeof(ui::Text), 0.5f, 5);

        math::vec2 pos(screen_rect.center().x, screen_rect.y - screen_rect.height * 0.3f);
        ptitle->set_obj_to_canvas_translation(pos);

        pos.y = screen_rect.y - screen_rect.height * 0.4f;
        psubtitle->set_obj_to_canvas_translation(pos);

        pmem = mem::allocate(sizeof(ui::Text));
        ui::Text *pplay_msg = new (pmem) ui::Text(*pcanvas, "press enter to plaY", sizeof(ui::Text), 0.28f, 5, 15);
        pos.y = screen_rect.center().y - screen_rect.height * 0.2f;
        pplay_msg->set_obj_to_canvas_translation(pos);

        pmem = mem::allocate(sizeof(ui::Text));
        ui::Text *pemail = new (pmem) ui::Text(*pcanvas, "mateusgondimlima@gmail.com", sizeof(ui::Text), 0.18f, 5, 15);
        pos.y = screen_rect.center().y - screen_rect.height * 0.4f;
        pemail->set_obj_to_canvas_translation(pos);

        return pcanvas;

}