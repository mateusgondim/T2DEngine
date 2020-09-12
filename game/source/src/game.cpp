
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

// UI
#include "UI_manager.hpp"
#include "Canvas.hpp"
#include "Widget.hpp"
#include "Text.hpp"

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

// UI creators
#include "Level_ui_creator.hpp"
#include "Main_menu_creator.hpp"
#include "Damage_region_creator.hpp"

//utility
#include "crc32.hpp"
#include "string_id.hpp"
#include "Path.hpp"
#include "Event.hpp"

#include "engine.hpp"

void load_resident_data()
{
    // load atlas needed for the player and enemies sprites
    Path sprites_path("resources/sprite sheets/");
    gfx::g_sprite_atlas_mgr.load("player",
                                 (sprites_path + "characters.xml").c_str(),
                                 &gfx::g_texture_2d_mgr);

    // load atlas needed for UI
    gfx::g_sprite_atlas_mgr.load("ui",
                                 (sprites_path + "gravitybold.xml").c_str(),
                                 &gfx::g_texture_2d_mgr);
}

int main(int argc, char *argv[])
{
    const char * pplevels[] = { "main_menu.tmx", "level01.tmx", "level02.tmx" };
    engine_init(3, 2, pplevels, 3);

    load_resident_data();

    /// Player setup

     // Get atlas needed for the player sprite
    gfx::Sprite_atlas *patlas = static_cast<gfx::Sprite_atlas*>(gfx::g_sprite_atlas_mgr.get_by_name("player"));

    string_id atlas_id = patlas->get_id();

    Player_creator *pplayer_creator = new Player_creator(atlas_id, 0);

    string_id player_type_id = intern_string("Player");

    // register the creator. CAREFULL PASSING UINT32_T , SHOULD BE A UINT16_T, FIX IT!
    gom::g_game_object_mgr.register_creator(player_type_id, pplayer_creator, SID('Player'));

    // set the player projectile creator
    physics_2d::Body_2d_def body_def;
    body_def.m_position = math::vec2();
    body_def.m_aabb = physics_2d::AABB_2d(math::vec2(-0.20f, -0.1f), math::vec2(0.20f, 0.1f));

    body_def.m_type = physics_2d::Body_2d::DYNAMIC;
    body_def.m_linear_velocity = math::vec2(11.0f, 0.0f);
    body_def.m_gravity_scale = 0.0f;
    body_def.m_map_collision = false;

    // set the player projectile anim_controller
    gfx::Animator_controller *pcontroller(new gfx::Animator_controller());
    gfx::Animation_player knife_idle_anim(gfx::Animation({ SID('fireball') }, 5));
    pcontroller->add_state("knife_idle_state", knife_idle_anim);

    string_id knife_type_id = intern_string("knife_obj");
    Projectile_creator *knife_projectile = new Projectile_creator(atlas_id, body_def, pcontroller);
    gom::g_game_object_mgr.register_creator(knife_type_id, knife_projectile, SID('p_projectile'));

    // Set the Hover Robor creator
    Hover_robot_creator * phover_robot_creator = new Hover_robot_creator(atlas_id, 0);

    //create a type id for the Hover_object. THIS SHOULD BE READ FROM A FILE
    string_id hover_robot_id = intern_string("hover_robot");

    //register the creator. CAREFULL PASSING UINT32_T , SHOULD BE A UINT16_T, FIX IT!
    gom::g_game_object_mgr.register_creator(hover_robot_id, phover_robot_creator, SID('Enemy'));

    // Register Damage_region creator
    Damage_region_creator * damage_region_creator = new Damage_region_creator();
    gom::g_game_object_mgr.register_creator(SID('damage_region'), damage_region_creator,
                                            SID('damage_region'));

    // register UI Creators
    Level_ui_creator * level_ui_creator = new Level_ui_creator(SID('ui'));
    gom::g_game_object_mgr.register_creator(SID('level_ui'), level_ui_creator, SID('canvas'));

    Main_menu_creator * main_menu_creator = new Main_menu_creator(SID('ui'));
    gom::g_game_object_mgr.register_creator(SID('main_menu'), main_menu_creator, SID('canvas'));

    // Set up the game's control scheme
    io::Keyboard_button_mapper & control_scheme = io::g_input_mgr.get_keyboard_control_scheme();
    control_scheme.map_action_to_button(SID('move_left'), io::Abstract_keyboard_index::KEY_LEFT);
    control_scheme.map_action_to_button(SID('move_right'), io::Abstract_keyboard_index::KEY_RIGHT);
    control_scheme.map_action_to_button(SID('move_down'), io::Abstract_keyboard_index::KEY_DOWN);
    control_scheme.map_action_to_button(SID('climb_down'), io::Abstract_keyboard_index::KEY_DOWN);
    control_scheme.map_action_to_button(SID('move_up'), io::Abstract_keyboard_index::KEY_UP);
    control_scheme.map_action_to_button(SID('climb_up'), io::Abstract_keyboard_index::KEY_UP);
    control_scheme.map_action_to_button(SID('jump'), io::Abstract_keyboard_index::KEY_A);
    control_scheme.map_action_to_button(SID('attack_01'), io::Abstract_keyboard_index::KEY_S);
    control_scheme.map_action_to_button(SID('action'), io::Abstract_keyboard_index::KEY_ENTER);

    // Load next Level - FOR DEBUGGING
    control_scheme.map_action_to_button(SID('next_level'), io::Abstract_keyboard_index::KEY_N);
    control_scheme.map_action_to_button(SID('previous_level'), io::Abstract_keyboard_index::KEY_P);

    level_management::g_level_mgr.request_level(0);

    gom::Camera_2d & main_camera = gom::g_game_object_mgr.get_main_camera();
    main_camera.track(player_type_id);

    gfx::Window * prender_window = gfx::g_graphics_mgr.get_render_window();

    bool just_paused = false;
    Event paused_event = Event(SID('GAME_PAUSED'));
    Event running_event = Event(SID('GAME_RUNNING'));

    while (!prender_window->should_close()) {
        level_management::g_level_mgr.tick();

        if (just_paused != level_management::g_level_mgr.is_game_clock_paused()) {
            just_paused = !just_paused;
            if (just_paused) {
                gom::g_game_object_mgr.broadcast_event(paused_event);
            }
            else {
                gom::g_game_object_mgr.broadcast_event(running_event);
            }
        }
        else {
            bool next_level_pressed = io::g_input_mgr.get_button_down(SID('next_level'));
            bool prev_level_pressed = io::g_input_mgr.get_button_down(SID('previous_level'));
            if (next_level_pressed) {
                level_management::g_level_mgr.request_next_level();
            }
        }
    }
    engine_shut_down();
    return 0;
}
