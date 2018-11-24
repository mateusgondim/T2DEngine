#include "Level_manager.hpp"

#include "Tile_map.hpp"
#include "Path.hpp"
#include "Shader.hpp"
#include "Shader_manager.hpp"
#include "Graphics_manager.hpp"
#include "Sprite_atlas.hpp"
#include "Texture_2d_manager.hpp"               
#include "Sprite_atlas_manager.hpp"

#include "input_manager.hpp"

#include "Game_object.hpp"
#include "Game_object_handle.hpp"
#include "Game_object_data.hpp"
#include "Camera_2d.hpp"
#include "Game_object_manager.hpp"
#include "Projectile_manager.hpp"

#include "World.hpp"
#include "Physics_manager.hpp"

#include "mat4.hpp"

#include <vector>

namespace gom
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

                // load atlas needed for the player and enemies sprite
                gfx::Sprite_atlas *patlas = static_cast<gfx::Sprite_atlas*>(gfx::g_sprite_atlas_mgr.load("player", (*m_presources_path +  "/sprite sheets/characters.xml").c_str(), &gfx::g_texture_2d_mgr));

                //load tile map shader  
                m_pmap_shader = static_cast<gfx::Shader*>(gfx::g_shader_mgr.load("tile_map_shader", (*m_presources_path + "/shaders/vertex.vert").c_str(), (*m_presources_path + "/shaders/fragment.frag").c_str()));

                //load sprite shader
                m_psprite_shader = static_cast<gfx::Shader*>(gfx::g_shader_mgr.load("sprite_shader", (*m_presources_path + "/shaders/sprite.vert").c_str(), (*m_presources_path + "/shaders/sprite.frag").c_str()));

                //load the data necessary to instantiate the level's game objects 
                load_level_objects();
        }


        /* Level_manager::init - This function makes some configurations necessary to play the current level, such as
         * setting shader values, initalizing the Camera and so on. It's also in this function that all
         * the game object in the level are instantiated.
         */
        void Level_manager::init()
        {
                //set the current and target aspect ratio of the screen
                m_curr_aspect_ratio = static_cast<float>(m_ptile_map->width()) / static_cast<float>(m_ptile_map->height());
                m_target_aspect_ratio = m_curr_aspect_ratio;

                //set variables to control the aspect ratio if the user changes the screen dimensions
                gfx::g_graphics_mgr.get_framebuffer_size(&m_prev_vport_width, &m_prev_vport_height);

                gfx::g_graphics_mgr.set_viewport(0, 0, m_prev_vport_width, m_prev_vport_height);
                m_curr_vport_width = m_prev_vport_width;
                m_curr_vport_height = m_prev_vport_height;

                //initialize camera
                float tile_wld_width = m_ptile_map->tile_width() / gfx::g_graphics_mgr.get_pixels_per_wld_unit();
                float tile_wld_height = m_ptile_map->tile_height() / gfx::g_graphics_mgr.get_pixels_per_wld_unit();
                math::vec2 map_origin = math::vec2(m_ptile_map->get_position().x, m_ptile_map->get_position().y);

                m_camera.init(tile_wld_width, tile_wld_height, gfx::g_graphics_mgr.get_tiles_per_screen_width(), gfx::g_graphics_mgr.get_tiles_per_screen_height(), m_ptile_map->width(), m_ptile_map->height(), map_origin);

                m_pmap_shader->uniform_matrix4fv(m_pmap_shader->get_uniform_location("V"), 1, false, m_camera.get_view().value_ptr());
                m_pmap_shader->uniform_matrix4fv(m_pmap_shader->get_uniform_location("P"), 1, false, m_camera.projection().value_ptr());
                m_pmap_shader->uniform_1f(m_pmap_shader->get_uniform_location("tileset1"), 0);
                gfx::g_graphics_mgr.set_tile_map_shader(m_pmap_shader);

                m_psprite_shader->uniform_matrix4fv(m_psprite_shader->get_uniform_location("V"), 1, false, m_camera.get_view().value_ptr());
                m_psprite_shader->uniform_matrix4fv(m_psprite_shader->get_uniform_location("P"), 1, false, m_camera.projection().value_ptr());
                m_psprite_shader->uniform_1f(m_psprite_shader->get_uniform_location("tileset"), 0);
                gfx::g_graphics_mgr.set_sprite_shader(m_psprite_shader);

                gfx::g_graphics_mgr.set_tile_map(m_ptile_map);

                // Initialize Input_manager
                io::map_action_to_button(io::GAME_ACTIONS::MOVE_LEFT, Button(io::KEYS::KEY_LEFT));
                io::map_action_to_button(io::GAME_ACTIONS::JUMP, Button(io::KEYS::KEY_A));
                io::map_action_to_button(io::GAME_ACTIONS::MOVE_RIGHT, Button(io::KEYS::KEY_RIGHT));
                io::map_action_to_button(io::GAME_ACTIONS::CLIMB_UP, Button(io::KEYS::KEY_UP));
                io::map_action_to_button(io::GAME_ACTIONS::CLIMB_DOWN, Button(io::KEYS::KEY_DOWN));
                io::map_action_to_button(io::GAME_ACTIONS::MOVE_UP, Button(io::KEYS::KEY_UP));
                io::map_action_to_button(io::GAME_ACTIONS::MOVE_DOWN, Button(io::KEYS::KEY_DOWN));
                io::map_action_to_button(io::GAME_ACTIONS::ATTACK_01, Button(io::KEYS::KEY_S));

                io::map_action_to_button(io::GAME_ACTIONS::RESET, Button(io::KEYS::KEY_R));

                instantiate_level_objects();

                //initialize timer
                m_timer.init();
                m_lag = 0.0f;
        }

        void Level_manager::tick()
        {
                std::cout << "FPS: " << m_timer.get_fps() << std::endl;

                m_timer.update();
                m_lag += m_timer.get_dt();

                float frame_time = m_timer.get_dt();

                const Button & restart_button = io::get_button_from_action(io::GAME_ACTIONS::RESET);
                if (restart_button.m_state == PRESSED) {
                        m_should_restart = true;
                }

                bool  lagging = (frame_time > m_dt) ? true : false;
                if (lagging) {
                        std::cout << "WE ARE BEHIND SCHEDULE by !!!!! " << (frame_time - m_dt) * 1000.0f << std::endl;
                }
                else {
                        std::cout << "WE ARE ON SCHEDULE" << std::endl;
                }

                while (m_lag >= m_dt) {
                        physics_2d::g_physics_mgr.get_world()->update(m_timer.get_fixed_dt());
                        m_lag -= m_dt;
                }

                //update the level's game objects
                gom::g_game_object_mgr.update_game_objects(m_timer.get_dt());
                //update the projectile manager
                gom::g_projectile_mgr.update(m_timer.get_dt());
                //update the level's camera
                m_camera.update(m_timer.get_dt());

                m_pmap_shader->uniform_matrix4fv(m_pmap_shader->get_uniform_location("V"), 1, false, m_camera.get_view().value_ptr());

                gfx::g_graphics_mgr.get_framebuffer_size(&m_curr_vport_width, &m_curr_vport_height);

                if ((m_curr_vport_width != m_prev_vport_width) || (m_curr_vport_height != m_prev_vport_height)) {
                        m_curr_aspect_ratio = (float)m_curr_vport_width / (float)m_curr_vport_height;

                        int width = m_curr_vport_width;
                        int height = (width / m_target_aspect_ratio + 0.5f);

                        if (height > m_curr_vport_height) {
                                height = m_curr_vport_height;
                                width = (height * m_target_aspect_ratio + 0.5f);
                        }
                        int vp_x = (m_curr_vport_width / 2.0f) - (width / 2);
                        int vp_y = (m_curr_vport_height / 2.0f) - (height / 2);
                        int vp_width = width + vp_x;
                        int vp_height = height + vp_y;
                        gfx::g_graphics_mgr.set_viewport(vp_x, vp_y, width, height);

                        m_prev_vport_width = m_curr_vport_width;
                        m_prev_vport_height = m_curr_vport_height;
                }

                m_psprite_shader->uniform_matrix4fv(m_psprite_shader->get_uniform_location("V"), 1, false, m_camera.get_view().value_ptr());

                gfx::g_graphics_mgr.render();

                if (m_should_restart) {
                        restart();
                }
        }

        void Level_manager::load_level_objects()
        {
                const Object_group *pgroup = m_ptile_map->get_object_group("Game_objects");
                if (pgroup) {
                        gom::Game_object_data data;
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
                std::vector<gom::Game_object_data>::const_iterator	game_obj_iter = m_level_data.cbegin();
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