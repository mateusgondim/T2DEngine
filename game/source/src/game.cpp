
#include "game.hpp"
#include <iostream>
#include <memory>


//math
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4.hpp"
#include "matrix_transform.hpp"

#include "Timer.hpp"
#include "Keyboard_button_mapper.hpp"
#include "Abstract_keyboard_index.hpp"
#include "Input_manager.hpp"

#include "Camera_2d.hpp"
#include "Tile_map.hpp"
#include "Shader.hpp"


//physics
#include "Body_2d_def.hpp"
#include "Body_2d.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"


#include "Player_idle_state.hpp"
#include "Player_running_state.hpp"
#include "Animation.hpp"
#include "Animation_player.hpp"

#include "Animator_controller.hpp"
#include "Animator_controller_parameter.hpp"
#include "Animator_state.hpp"
#include "Animator_state_transition.hpp"
#include "Animator_condition.hpp"
#include "Player.hpp"

//Graphics
#include "Sprite.hpp"
#include "Shader_manager.hpp"
#include "Texture_2d_manager.hpp"
#include "Sprite_atlas.hpp"
#include "Sprite_atlas_manager.hpp"
#include "Window.hpp"
#include "Graphics_manager.hpp"

//gom
#include "Game_object_handle.hpp"
#include "Game_object_manager.hpp"
#include "Level_manager.hpp"

#include "Projectile_manager.hpp"
#include "Player_creator.hpp"
#include "Projectile_creator.hpp"

#include "Hover_robot.hpp"
#include "Hover_robot_creator.hpp"

//memory
#include "runtime_memory_allocator.hpp"

//utility
#include "crc32.hpp"
#include "string_id.hpp"
#include "Path.hpp"

#include "engine.hpp"

int main(int argc, char *argv[])
{
        Path resources_path("../resources", Path::FORWARD_SLASH);
        // Load tile map. OBS: WE NEED TO CHANGE THIS LATTER... THE MAP LOADING SHOULD BE DONE ON ENGINE.INIT 
        Tile_map tile_map((resources_path + "/maps/level01.tmx").c_str());
        print_tile_map(std::cout, tile_map) << std::endl;
        
        engine_init(3, 2, &tile_map);

       /// Player setup

        // Get atlas needed for the player sprite
        gfx::Sprite_atlas *patlas = static_cast<gfx::Sprite_atlas*>(gfx::g_sprite_atlas_mgr.get_by_name("player"));

        //get the atlas resource id
        string_id atlas_id = patlas->get_id();

        // Create the Player Creator class
        Player_creator *pplayer_creator = new Player_creator(atlas_id, 0);

        //create a type id for the player object. THIS SHOULD BE READ FROM A FILE. MUST BE THE SAME ID THAT IS STORED ON THE TMX FILE
        string_id player_type_id = intern_string("Player");

        //register the creator. CAREFULL PASSING UINT32_T , SHOULD BE A UINT16_T, FIX IT!
        gom::g_game_object_mgr.register_creator(player_type_id, pplayer_creator, SID('Player'));

        // set the player projectile creator
        physics_2d::Body_2d_def body_def;
        body_def.m_position = math::vec2();
        body_def.m_aabb = physics_2d::AABB_2d(math::vec2(-0.20f, -0.1f),
                                              math::vec2(0.20f, 0.1f));

        body_def.m_type = physics_2d::Body_2d::DYNAMIC;
        body_def.m_linear_velocity = math::vec2(11.0f, 0.0f);
        body_def.m_gravity_scale = 0.0f;
        body_def.m_map_collision = false;

        // set the player projectile anim_controller
        gfx::Animator_controller *pcontroller(new gfx::Animator_controller());
        gfx::Animation_player knife_idle_anim(gfx::Animation({ 19 }, 5));
        pcontroller->add_state("knife_idle_state", knife_idle_anim);

        //create a type id for the object
        string_id knife_type_id = intern_string("knife_obj");
        //creator
        Projectile_creator *knife_projectile = new Projectile_creator(atlas_id,
                                                                      body_def, pcontroller);
        gom::g_game_object_mgr.register_creator(knife_type_id, knife_projectile, 
                                                SID('p_projectile'));

        // Set the Hover Robor creator
        Hover_robot_creator * phover_robot_creator = new Hover_robot_creator(atlas_id, 0);

        //create a type id for the Hover_object. THIS SHOULD BE READ FROM A FILE
        string_id hover_robot_id = intern_string("hover_robot");

        //register the creator. CAREFULL PASSING UINT32_T , SHOULD BE A UINT16_T, FIX IT!
        gom::g_game_object_mgr.register_creator(hover_robot_id, phover_robot_creator, SID('Enemy'));

        std::cout << " TILE MAP WIDTH = " << tile_map.width() << "| TILE MAP HEIGHT = "
                  << tile_map.height() << std::endl;

        level_management::g_level_mgr.init();

        //Set up the game's control scheme
        io::Keyboard_button_mapper & control_scheme = io::g_input_mgr.get_keyboard_control_scheme();
        control_scheme.map_action_to_button(SID('move_left'),
                                            io::Abstract_keyboard_index::KEY_LEFT);
        control_scheme.map_action_to_button(SID('move_right'), 
                                            io::Abstract_keyboard_index::KEY_RIGHT);
        control_scheme.map_action_to_button(SID('move_down'),
                                            io::Abstract_keyboard_index::KEY_DOWN);
        control_scheme.map_action_to_button(SID('climb_down'),
                                            io::Abstract_keyboard_index::KEY_DOWN);
        control_scheme.map_action_to_button(SID('move_up'),
                                            io::Abstract_keyboard_index::KEY_UP);
        control_scheme.map_action_to_button(SID('climb_up'),
                                            io::Abstract_keyboard_index::KEY_UP);
        control_scheme.map_action_to_button(SID('jump'),
                                            io::Abstract_keyboard_index::KEY_A);
        control_scheme.map_action_to_button(SID('attack_01'),
                                            io::Abstract_keyboard_index::KEY_S);

        gom::g_level_mgr.get_camera().track(player_type_id);

        gfx::Window * prender_window = gfx::g_graphics_mgr.get_render_window();
        while (!prender_window->should_close()) {
                level_management::g_level_mgr.tick();
        }
        engine_shut_down();
        return 0;
}
