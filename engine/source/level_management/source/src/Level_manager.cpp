#include "Level_manager.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include "Tile_map.hpp"
#include "Path.hpp"
#include "Shader.hpp"
#include "Shader_manager.hpp"
#include "Window.hpp"
#include "Graphics_manager.hpp"
#include "Sprite_atlas.hpp"
#include "Texture_2d_manager.hpp"               
#include "Sprite_atlas_manager.hpp"

#include "Keyboard_button_mapper.hpp"
#include "Abstract_keyboard_index.hpp"
#include "Input_manager.hpp"

#include "Resource.hpp"

#include "Game_object.hpp"
#include "Game_object_handle.hpp"
#include "Game_object_data.hpp"
#include "Camera_2d.hpp"
#include "Game_object_manager.hpp"
#include "Projectile_manager.hpp"

#include "World.hpp"
#include "Physics_manager.hpp"

#include "mat4.hpp"
#include "crc32.hpp"

#include "Widget.hpp"
#include "Canvas.hpp"
#include "UI_manager.hpp"

#include <vector>
#include <utility>

namespace level_management
{
        Level_manager g_level_mgr;

        const float Level_manager::m_dt = 1.0F / 60.0F;

        /* Level_manager::load- This function is responsable to load all the necessary data to play
         * the current level. The main entry point to get all the data is the TMX file, this file stores not only the position and id of all the game objects in the level,
         * but also the location of all the resources necessary to play level, such as textures, animator controllers, sprite atlases, shaders and so on.
         */
        void Level_manager::load(Path & resources_path, Tile_map *ptile_map)
        {
                m_presources_path = new Path(resources_path);
                m_ptile_map = ptile_map;

                // load atlas needed for the player and enemies sprites
                Path sprites_path = *m_presources_path + "/sprite sheets/";
                gfx::g_sprite_atlas_mgr.load("player",
                                             (sprites_path + "characters.xml").c_str(),
                                             &gfx::g_texture_2d_mgr);

                // load atlas needed for UI
                gfx::g_sprite_atlas_mgr.load("ui",
                                             (sprites_path + "c64.xml").c_str(),
                                             &gfx::g_texture_2d_mgr);

                //load tile map shader  
                Path shaders_path = *m_presources_path + "/shaders/";
                rms::Resource *pmap_shader_res = gfx::g_shader_mgr.load("tile_map_shader",
                                                                        (shaders_path + "vertex.vert").c_str(),
                                                                        (shaders_path + "fragment.frag").c_str());
                m_pmap_shader = static_cast<gfx::Shader*>(pmap_shader_res);

                //load sprite shader
                rms::Resource *psprite_shader_res = gfx::g_shader_mgr.load("sprites_shader",
                                                                           (shaders_path + "sprite.vert").c_str(),
                                                                           (shaders_path + "sprite.frag").c_str());
                m_psprite_shader = static_cast<gfx::Shader*>(psprite_shader_res);

                //---------------------Test for UI---------------------------------------------//
                rms::Resource * pui_shader_res = gfx::g_shader_mgr.load("ui_shader",
                                                                        (shaders_path + "ui.vert").c_str(),
                                                                        (shaders_path + "ui.frag").c_str());
                
                ui::g_ui_mgr.set_widgets_shader(*(static_cast<gfx::Shader*>(pui_shader_res)));

                //load the data necessary to instantiate the level's game objects 
                load_level_objects();
        }


        /* Level_manager::init - This function makes some configurations necessary to play the current level, such as
         * setting shader values, initalizing the Camera and so on. It's also in this function that all
         * the game object in the level are instantiated.
         */
        void Level_manager::init()
        {
                //initialize camera
                float tile_wld_width = m_ptile_map->tile_width() / gfx::g_graphics_mgr.get_pixels_per_wld_unit();
                float tile_wld_height = m_ptile_map->tile_height() / gfx::g_graphics_mgr.get_pixels_per_wld_unit();
                math::vec2 map_origin = math::vec2(m_ptile_map->get_position().x, m_ptile_map->get_position().y);

                gom::Camera_2d *plevel_camera = new gom::Camera_2d(tile_wld_width, tile_wld_height,
                                                                   gfx::g_graphics_mgr.get_tiles_per_screen_width(),
                                                                   gfx::g_graphics_mgr.get_tiles_per_screen_height(), 
                                                                   m_ptile_map->width(),
                                                                   m_ptile_map->height(), map_origin);

                m_tile_map_view_loc = m_pmap_shader->get_uniform_location("V");

                m_pmap_shader->uniform_matrix4fv(m_tile_map_view_loc, 1, false,
                                                 plevel_camera->get_view().value_ptr());

                m_pmap_shader->uniform_matrix4fv(m_pmap_shader->get_uniform_location("P"), 1, false,
                                                 plevel_camera->projection().value_ptr());

                m_pmap_shader->uniform_1f(m_pmap_shader->get_uniform_location("tileset1"), 0);
                gfx::g_graphics_mgr.set_tile_map_shader(m_pmap_shader);

                m_sprites_view_loc = m_psprite_shader->get_uniform_location("V");

                m_psprite_shader->uniform_matrix4fv(m_sprites_view_loc, 1, false,
                                                    plevel_camera->get_view().value_ptr());

                m_psprite_shader->uniform_matrix4fv(m_psprite_shader->get_uniform_location("P"), 1,
                                                    false, plevel_camera->projection().value_ptr());

                m_psprite_shader->uniform_1f(m_psprite_shader->get_uniform_location("tileset"), 0);
                gfx::g_graphics_mgr.set_sprite_shader(m_psprite_shader);

                // set as the main camera
                gom::g_game_object_mgr.set_main_camera(plevel_camera);

                gfx::g_graphics_mgr.set_tile_map(m_ptile_map);

                // Initialize the default control scheme
                io::Keyboard_button_mapper & control_scheme = io::g_input_mgr.get_keyboard_control_scheme();

                control_scheme.map_action_to_button(SID('pause'), io::Abstract_keyboard_index::KEY_P);
                control_scheme.map_action_to_button(SID('reset_level'), 
                                                    io::Abstract_keyboard_index::KEY_R);

                // WIDGET TEST
                math::Rect screen_rect = plevel_camera->get_screen_rect();
                rms::Resource * pres = gfx::g_sprite_atlas_mgr.get_by_name("ui");
                gfx::Sprite_atlas * patlas = static_cast<gfx::Sprite_atlas*>(pres);
                static_cast<gfx::Sprite_atlas*>(pres);
                ui::Canvas *pcanvas = ui::g_ui_mgr.create_canvas(screen_rect, *patlas);

                math::Rect widget_rect(screen_rect.x, screen_rect.y,
                                       screen_rect.width / 4.0f, screen_rect.height / 6.0f);
                ui::Widget *pwidget = pcanvas->create_widget(widget_rect);
                

                instantiate_level_objects();

                //initialize timers
                Timer::init();
                // m_timer.init();
                m_last_time_cycles = glfwGetTimerValue();
                m_lag = 0.0f;
        }

        void Level_manager::tick()
        {
                io::g_input_mgr.poll_events();

                bool pause_pressed = io::g_input_mgr.get_button_down(SID('pause'));

                bool is_paused = m_timer.is_paused();
                if (pause_pressed) {
                        is_paused = !is_paused;
                        m_timer.set_paused(is_paused);
                }
                //std::cout << "FPS: " << m_timer.get_fps() << std::endl;
               //std::cout << "FRAME TIME: " << m_timer.get_dt() << std::endl;

                //////////////////////////////////////////
                m_curr_time_cycles = glfwGetTimerValue();
                m_delta_time_seconds = Timer::cycles_to_seconds(m_curr_time_cycles - m_last_time_cycles);
                m_last_time_cycles = m_curr_time_cycles;


                uint64_t last_game_time_cycles = m_timer.get_time_cycles();
                m_timer.update(m_delta_time_seconds);
                float game_delta_time_seconds = Timer::cycles_to_seconds(m_timer.get_time_cycles() - last_game_time_cycles);

                ////////////////////////////////////////
                // m_lag += m_timer.get_dt();
                m_lag += game_delta_time_seconds;

                //float frame_time = m_timer.get_dt();

                bool restart_pressed = io::g_input_mgr.get_button_down(SID('reset_level'));
                if (restart_pressed) {
                        m_should_restart = true;
                }

                //bool  lagging = (frame_time > m_dt) ? true : false;
                //if (lagging) {
                //        std::cout << "WE ARE BEHIND SCHEDULE by !!!!! " << (frame_time - m_dt) * 1000.0f << std::endl;
                //}
                //else {
                //        std::cout << "WE ARE ON SCHEDULE" << std::endl;
                //}

                while (m_lag >= m_dt) {
                        physics_2d::g_physics_mgr.get_world()->update(m_timer.get_fixed_delta_time_seconds());
                        m_lag -= m_dt;
                }

                //update the level's game objects
                gom::g_game_object_mgr.update_game_objects(game_delta_time_seconds);

                //update the projectile manager
                gom::g_projectile_mgr.update(game_delta_time_seconds);

                //update the level's camera
                gom::Camera_2d * plevel_camera = gom::g_game_object_mgr.get_main_camera();
                
                plevel_camera->update(game_delta_time_seconds);
                
                m_pmap_shader->uniform_matrix4fv(m_tile_map_view_loc, 1, false,
                                                 plevel_camera->get_view().value_ptr());

                gfx::Window * prender_window = gfx::g_graphics_mgr.get_render_window();
                prender_window->update_viewport();

                m_psprite_shader->uniform_matrix4fv(m_sprites_view_loc, 1, false,
                                                    plevel_camera->get_view().value_ptr());

                gfx::g_graphics_mgr.render();

                ui::g_ui_mgr.render();

                gfx::g_graphics_mgr.swap_and_poll();

                if (m_should_restart) {
                        restart();
                }
        }

        void Level_manager::load_level_objects()
        {
                const Object_group *pgroup = m_ptile_map->get_object_group("Game_objects");
                if (pgroup) {
                        Game_object_data data;
                        math::vec2	pos;

                        const std::vector<Object*> & objects = pgroup->get_objects();
                        std::vector<Object*>::const_iterator it;
                        for (it = objects.cbegin(); it != objects.cend(); ++it) {
                                const char *	 pobj_type = (*it)->get_type();
                                pos = m_ptile_map->pixels_to_wld_coord((*it)->get_x(), (*it)->get_y());
                                data.object_type_id = intern_string(pobj_type);
                                data.position = math::vec3(pos);
                                m_level_data.push_back(data);
                                std::cout << "Object  type \'" << pobj_type << "\' = " << data.object_type_id
                                        << "position = " << data.position << std::endl;
                        }
                }
        }

        void Level_manager::instantiate_level_objects()
        {
                //create the level's objects
                std::vector<Game_object_data>::const_iterator	game_obj_iter = m_level_data.cbegin();
                //gom::g_game_object_mgr.instantiate(game_obj_iter->object_type_id, game_obj_iter);
                for (; game_obj_iter != m_level_data.cend(); ++game_obj_iter) {
                        gom::g_game_object_mgr.instantiate(game_obj_iter->object_type_id, game_obj_iter->position);
                }
        }

        void Level_manager::restart()
        {
                m_should_restart = false;
                std::cout << "RESETING LEVEL...." << std::endl;

                //restart projectile manager
                gom::g_projectile_mgr.reset();

                //1. reset Game_object_manager
                gom::g_game_object_mgr.reset();

                //RESET TIMER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //instantiate the game objects of the level
                instantiate_level_objects();
        }
// REMEMBER THAT WE ARE NOT CALLING THIS DESTRUCTOR ANYHWERE!!!! MAYBE WE SHOULD MOVE THIS CODE TO THE SHUTDOWN FUNCTION.....
        Level_manager::~Level_manager()
        {
                delete m_presources_path;
        }
}