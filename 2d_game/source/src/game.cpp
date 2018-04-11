
#include "game.hpp"

#include <iostream>
#include <memory>


//math
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4.hpp"
#include "matrix_transform.hpp"

#include "Timer.hpp"

#include "Camera_2d.hpp"
#include "Tile_map.hpp"
#include "Shader.hpp"
#include "tmx_parser.hpp"


//physics
#include "Body_2d.hpp"
#include "Collision_listener.hpp"
#include "Game_coll_listener.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"

#include "Button.hpp"
#include "Input_manager.hpp"

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

#include "string_id.hpp"

#include <iostream>

#include "Engine.hpp"



#define FRAME_RATE 60.0f

float curr_time;
float last_time;
float dt = 1.0F / FRAME_RATE;


void error_callback(int error, const char * descr)
{
	std::cerr << "GLFW ERROR: " << descr << std::endl;
}

void key_callback(int key, int scancode, int action, int mods)
{
	g_engine.m_input_manager.key_callback(key, scancode, action, mods);
}

gfx::Animator_controller *get_player_anim_controller() 
{
	gfx::Animator_controller *pcontroller(new gfx::Animator_controller());

	//create the animation players for each animation state
	gfx::Animation_player player_idle_anim( gfx::Animation({5}, 5));
	gfx::Animation_player player_running_anim(gfx::Animation({8, 10, 9}, 10));
	gfx::Animation_player player_jumping_anim(gfx::Animation({6}, 1));
	gfx::Animation_player player_climbing_anim(gfx::Animation({ 1, 2 }, 6)); // gfx::Animation({3 }, 6) );
	gfx::Animation_player player_finishing_climbing_anim(gfx::Animation({ 3 }, 6));
	gfx::Animation_player player_attacking_anim( gfx::Animation({ 13, 14 }, 10, false) );
	gfx::Animation_player player_ducking_idle_anim( gfx::Animation({4}, 1));
	gfx::Animation_player player_ducking_attacking_anim(gfx::Animation({ 11, 12 }, 10, false));

	//add the parameters to the controller
	//pcontroller->add_bool_param("is_running");
	pcontroller->add_parameter("is_running", gfx::Animator_controller_parameter::Type::BOOL);

	//pcontroller->add_bool_param("is_jumping");
	pcontroller->add_parameter("is_jumping", gfx::Animator_controller_parameter::Type::BOOL);

	//pcontroller->add_bool_param("is_climbing");
	pcontroller->add_parameter("is_climbing", gfx::Animator_controller_parameter::Type::BOOL);

	//pcontroller->add_bool_param("is_ducking");
	pcontroller->add_parameter("is_ducking", gfx::Animator_controller_parameter::Type::BOOL);

	//pcontroller->add_trigger_param("is_attacking");
	pcontroller->add_parameter("is_attacking", gfx::Animator_controller_parameter::Type::TRIGGER);

	pcontroller->add_parameter("finish_climbing", gfx::Animator_controller_parameter::Type::BOOL);

	//gfx::Animation_state player_idle_state("player_idle", player_idle_anim);
	gfx::Animator_state & player_idle_state = pcontroller->add_state("player_idle", player_idle_anim);

	//gfx::Animation_state player_running_state("player_running", player_running_anim);
	gfx::Animator_state & player_running_state = pcontroller->add_state("player_running", player_running_anim);

	//gfx::Animation_state player_jumping_state("player_jumping", player_jumping_anim);
	gfx::Animator_state & player_jumping_state = pcontroller->add_state("player_jumping", player_jumping_anim);

	//gfx::Animation_state player_climbing_state("player_climbing", player_climbing_anim);
	gfx::Animator_state & player_climbing_state = pcontroller->add_state("player_climbing", player_climbing_anim);

	gfx::Animator_state & player_finishing_climbing_state = pcontroller->add_state("player_finishing_climbing", player_finishing_climbing_anim);

	//gfx::Animation_state player_attacking_state("player_attacking", player_attacking_anim);
	gfx::Animator_state & player_attacking_state = pcontroller->add_state("player_attacking", player_attacking_anim);
	
	//gfx::Animation_state player_ducking_idle_state("player_ducking_idle", player_ducking_idle_anim);
	gfx::Animator_state & player_ducking_idle_state = pcontroller->add_state("player_ducking_idle", player_ducking_idle_anim);

	//gfx::Animation_state player_ducking_attacking_state("player_ducking_attacking", player_ducking_attacking_anim);
	gfx::Animator_state & player_ducking_attacking_state = pcontroller->add_state("player_ducking_attacking", player_ducking_attacking_anim);

	//transitions from player idle


	//gfx::Transition idle_to_ducking_idle("idle_to_ducking_idle", "player_idle", "player_ducking_idle");
	//pcontroller->add_transition(idle_to_ducking_idle);
	//pcontroller->add_bool_condition("idle_to_ducking_idle", "is_ducking", true);

	gfx::Animator_state_transition & idle_to_ducking_idle = player_idle_state.add_transition("player_ducking_idle");
	idle_to_ducking_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_ducking");

	//gfx::Transition attack_from_idle("idle_to_attack", "player_idle", "player_attacking");
	//pcontroller->add_transition(attack_from_idle);
	//pcontroller->add_trigger_condition("idle_to_attack", "is_attacking", true);

	gfx::Animator_state_transition & idle_to_attack = player_idle_state.add_transition("player_attacking");
	idle_to_attack.add_condition(gfx::Animator_condition::Mode::IF, 1, "is_attacking");

	//gfx::Transition start_running("start_running", "player_idle", "player_running");
	//pcontroller->add_transition(start_running);
	//pcontroller->add_bool_condition("start_running", "is_running", true);

	gfx::Animator_state_transition & idle_to_running = player_idle_state.add_transition("player_running");
	idle_to_running.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_running");

	//gfx::Transition climbing_from_idle("climbing_from_idle", "player_idle", "player_climbing");
	//pcontroller->add_transition(climbing_from_idle);
	//pcontroller->add_bool_condition("climbing_from_idle", "is_climbing", true);

	gfx::Animator_state_transition & idle_to_climbing = player_idle_state.add_transition("player_climbing");
	idle_to_climbing.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_climbing");

	//gfx::Transition start_jumping_from_idle("start_jumping_from_idle", "player_idle", "player_jumping");
	//pcontroller->add_transition(start_jumping_from_idle);
	//pcontroller->add_bool_condition("start_jumping_from_idle", "is_jumping", true);

	gfx::Animator_state_transition & idle_to_jumping = player_idle_state.add_transition("player_jumping");
	idle_to_jumping.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_jumping");


	//gfx::Transition running_to_attack("running_to_attack", "player_running", "player_attacking");
	//pcontroller->add_transition(running_to_attack);
	//pcontroller->add_trigger_condition("running_to_attack", "is_attacking", true);

	gfx::Animator_state_transition & running_to_attack = player_running_state.add_transition("player_attacking");
	running_to_attack.add_condition(gfx::Animator_condition::Mode::IF, 1, "is_attacking");

	//gfx::Transition stop_running("stop_running", "player_running", "player_idle");
	//pcontroller->add_transition(stop_running);
	//pcontroller->add_bool_condition("stop_running", "is_running", false);
	//pcontroller->add_trigger_condition("stop_running", "is_attacking", false);

	gfx::Animator_state_transition & running_to_idle = player_running_state.add_transition("player_idle");
	running_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_running");
	running_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_jumping");
	//running_to_idle.add_condition(gfx::Animator_condition::Mode::IF_NOT, 0, "is_attacking");

	//gfx::Transition start_jumping_from_running("start_jumping_from_running", "player_running", "player_jumping");
	//pcontroller->add_transition(start_jumping_from_running);
	//pcontroller->add_bool_condition("start_jumping_from_running", "is_jumping", true);

	gfx::Animator_state_transition & running_to_jumping = player_running_state.add_transition("player_jumping");
	running_to_jumping.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_jumping");
	running_to_jumping.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_running");

	//gfx::Transition jumping_to_attack("jumping_to_attack", "player_jumping", "player_attacking");
	//pcontroller->add_transition(jumping_to_attack);
	//pcontroller->add_trigger_condition("jumping_to_attack", "is_attacking", true);

	gfx::Animator_state_transition & jumping_to_attacking = player_jumping_state.add_transition("player_attacking");
	jumping_to_attacking.add_condition(gfx::Animator_condition::Mode::IF, 1, "is_attacking");

	//gfx::Transition stop_jumping_to_idle("stop_jumping_to_idle", "player_jumping", "player_idle");
	//pcontroller->add_transition(stop_jumping_to_idle);
	//pcontroller->add_bool_condition("stop_jumping_to_idle", "is_jumping", false);
	//pcontroller->add_bool_condition("stop_jumping_to_idle", "is_running", false);
	//pcontroller->add_bool_condition("stop_jumping_to_idle", "is_climbing", false);

	gfx::Animator_state_transition & jumping_to_idle = player_jumping_state.add_transition("player_idle");
	jumping_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_jumping");
	jumping_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_running");
	jumping_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_climbing");


	//gfx::Transition stop_jumping_to_running("stop_jumping_to_running", "player_jumping", "player_running");
	//pcontroller->add_transition(stop_jumping_to_running);
	//pcontroller->add_bool_condition("stop_jumping_to_running", "is_jumping", false);
	//pcontroller->add_bool_condition("stop_jumping_to_running", "is_running", true);

	gfx::Animator_state_transition & jumping_to_running = player_jumping_state.add_transition("player_running");
	jumping_to_running.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_jumping");
	jumping_to_running.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_running");


	//gfx::Transition climbing_from_jumping("climbing_from_jumping", "player_jumping", "player_climbing");
	////pcontroller->add_transition(climbing_from_jumping);
	//pcontroller->add_bool_condition("climbing_from_jumping", "is_climbing", true);
	//pcontroller->add_bool_condition("climbing_from_jumping", "is_jumping", false);

	gfx::Animator_state_transition & jumping_to_climbing = player_jumping_state.add_transition("player_climbing");
	jumping_to_climbing.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_climbing");
	jumping_to_climbing.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_jumping");



	//this transition is gonna go, should be from ending climb
	//gfx::Transition idle_from_climbing("idle_from_climbing", "player_climbing", "player_idle");
	//pcontroller->add_transition(idle_from_climbing);
	//pcontroller->add_bool_condition("idle_from_climbing", "is_climbing", false);
	//pcontroller->add_bool_condition("idle_from_climbing", "is_jumping", false);

	gfx::Animator_state_transition & climbing_to_finish_climbing = player_climbing_state.add_transition("player_finishing_climbing");
	climbing_to_finish_climbing.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "finish_climbing");


	gfx::Animator_state_transition & climbing_to_idle = player_climbing_state.add_transition("player_idle");
	climbing_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_climbing");
	climbing_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_jumping");

	//gfx::Transition jumping_from_climbing("jumping_from_climbing", "player_climbing", "player_jumping");
	//pcontroller->add_transition(jumping_from_climbing);
	//pcontroller->add_bool_condition("jumping_from_climbing", "is_climbing", false);
	//pcontroller->add_bool_condition("jumping_from_climbing", "is_jumping", true);

	gfx::Animator_state_transition & climbing_to_jumping = player_climbing_state.add_transition("player_jumping");
	climbing_to_jumping.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_climbing");
	climbing_to_jumping.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_jumping");

	gfx::Animator_state_transition & finish_climbing_to_idle = player_finishing_climbing_state.add_transition("player_idle");
	finish_climbing_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_climbing");
	finish_climbing_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "finish_climbing");

	gfx::Animator_state_transition & finish_climbing_to_climbing = player_finishing_climbing_state.add_transition("player_climbing");
	finish_climbing_to_climbing.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_climbing");
	finish_climbing_to_climbing.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "finish_climbing");

	//gfx::Transition idle_from_attacking("attack_to_idle", "player_attacking", "player_idle");
	//pcontroller->add_transition(idle_from_attacking);
	//pcontroller->add_trigger_condition("attack_to_idle", "is_attacking", false);
	//pcontroller->add_bool_condition("attack_to_idle", "is_jumping", false);

	gfx::Animator_state_transition & attacking_to_idle = player_attacking_state.add_transition("player_idle");
	attacking_to_idle.add_condition(gfx::Animator_condition::Mode::IF_NOT, 0, "is_attacking");
	attacking_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_jumping");

	//gfx::Transition attack_to_jumping("attack_to_jumping", "player_attacking", "player_jumping");
	//pcontroller->add_transition(attack_to_jumping);
	//pcontroller->add_trigger_condition("attack_to_jumping", "is_attacking", false);
	//pcontroller->add_bool_condition("attack_to_jumping", "is_jumping", true);

	gfx::Animator_state_transition & attacking_to_jumping = player_attacking_state.add_transition("player_jumping");
	attacking_to_jumping.add_condition(gfx::Animator_condition::Mode::IF_NOT, 0, "is_attacking");
	attacking_to_jumping.add_condition(gfx::Animator_condition::Mode::EQUALS, 1, "is_jumping");






	//gfx::Transition ducking_idle_to_idle("ducking_idle_to_idle", "player_ducking_idle", "player_idle");
	//pcontroller->add_transition(ducking_idle_to_idle);
	//pcontroller->add_bool_condition("ducking_idle_to_idle", "is_ducking", false);

	gfx::Animator_state_transition & ducking_idle_to_idle = player_ducking_idle_state.add_transition("player_idle");
	ducking_idle_to_idle.add_condition(gfx::Animator_condition::Mode::EQUALS, 0, "is_ducking");

	//gfx::Transition ducking_idle_to_ducking_attacking("ducking_idle_to_ducking_attacking", "player_ducking_idle", "player_ducking_attacking");
	//pcontroller->add_transition(ducking_idle_to_ducking_attacking);
	//pcontroller->add_trigger_condition("ducking_idle_to_ducking_attacking", "is_attacking", true);

	gfx::Animator_state_transition & ducking_idle_to_ducking_attacking = player_ducking_idle_state.add_transition("player_ducking_attacking");
	ducking_idle_to_ducking_attacking.add_condition(gfx::Animator_condition::Mode::IF, 1, "is_attacking");

	//gfx::Transition ducking_attacking_to_ducking_idle("ducking_attacking_to_ducking_idle", "player_ducking_attacking", "player_ducking_idle");
	//pcontroller->add_transition(ducking_attacking_to_ducking_idle);
	//pcontroller->add_trigger_condition("ducking_attacking_to_ducking_idle", "is_attacking", false);

	gfx::Animator_state_transition & ducking_attacking_to_ducking_idle = player_ducking_attacking_state.add_transition("player_ducking_idle");
	ducking_attacking_to_ducking_idle.add_condition(gfx::Animator_condition::Mode::IF_NOT, 0, "is_attacking");

	std::cout << (*pcontroller);
	return pcontroller;
}


int main(int argc, char *argv[])
{
	g_engine.m_graphics_manager.set_error_callback(error_callback);
	
	// Load tile map
	Tile_map tile_map;

	std::string path = argv[1];

	if (!load_tile_map(path, tile_map)) {
		std::cout << "Tile_map was loaded " << std::endl;
		print_tile_map(std::cout, tile_map);
	}
	else {
		std::cout << "ERROR: Could not load Tile_map " << std::endl;
		return -1;
	}
	
	g_engine.init(4, 3, &tile_map);

	g_engine.m_graphics_manager.create_window(512, 480, "2D Game");

	
	//glfwSetKeyCallback(window, key_callback);
	g_engine.m_graphics_manager.set_key_callback(key_callback);

	int vport_width, vport_height;

	//glfwGetFramebufferSize(window, &vport_width, &vport_height);
	g_engine.m_graphics_manager.get_framebuffer_size(&vport_width, &vport_height);

	//glViewport(0, 0, vport_width, vport_height);
	g_engine.m_graphics_manager.set_viewport(0, 0, vport_width, vport_height);

	
	// CHANGE ABSOLUTE PATH!!!!!!!
	
	string_id     tile_map_shader_id = g_engine.m_graphics_manager.load_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/vertex.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/fragment.frag");
	gfx::Shader   *ptile_map_shader = g_engine.m_graphics_manager.get_shader(tile_map_shader_id);

	//load the data to render the map into the graphics manager
	g_engine.m_graphics_manager.set_tile_map_renderer(&tile_map);

	//Tile_map_renderer map_renderer(tile_map, *ptile_map_shader );

	math::mat4 V;
	
	//GLint v_loc, p_loc;
	int32_t v_loc, p_loc;

	//GLint sampler_loc;
	int32_t sampler_loc;

	gfx::Camera_2d camera(1.0f, 1.0f, 16, 15);

	//cgm::vec3 t(-floor(tile_map.width() / (float)2), -floor(tile_map.height() / (float)2), 0.0f);
    //V = cgm::rotate_y(180.0f);
	//V.transpose();
	//V = cgm::mat4(cgm::mat4(), t);

	//P = cgm::ortho(-floor(tile_map.width() / (float)2),  ceil(tile_map.width() / (float)2), -floor(tile_map.height() / (float)2), ceil(tile_map.height() / (float)2), -1.0, 1.0f);
	math::vec3 p = camera.get_position();
	p.x = -p.x;
	p.y = -p.y;
	V = math::mat4(math::mat4(), p);

	v_loc = ptile_map_shader->get_uniform_location("V");
	p_loc = ptile_map_shader->get_uniform_location("P");
	sampler_loc = ptile_map_shader->get_uniform_location("tileset1");

	//set uniforms on current shader program
	g_engine.m_graphics_manager.set_current_shader_program(tile_map_shader_id);
	g_engine.m_graphics_manager.uniform_matrix4fv(v_loc, 1, false, V.value_ptr());
    g_engine.m_graphics_manager.uniform_matrix4fv(p_loc, 1, false, camera.projection().value_ptr());
	g_engine.m_graphics_manager.uniform_1f(sampler_loc, 0);

	///////////////////--------------------------Initialization code------------------ ///////////////////////////////////////////////////////////////////////////
	//Set physics engine collision listener
	Game_coll_listener game_coll_listener;
	g_engine.m_physics_manager.get_world()->set_collision_listener(&game_coll_listener);
	
	// Initialize Input_manager
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::MOVE_LEFT, Button(Input_manager::KEYS::KEY_LEFT));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::JUMP, Button(Input_manager::KEYS::KEY_A));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::MOVE_RIGHT, Button(Input_manager::KEYS::KEY_RIGHT));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::CLIMB_UP, Button(Input_manager::KEYS::KEY_UP));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::CLIMB_DOWN, Button(Input_manager::KEYS::KEY_DOWN));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::MOVE_UP, Button(Input_manager::KEYS::KEY_UP));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::MOVE_DOWN, Button(Input_manager::KEYS::KEY_DOWN));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::ATTACK_01, Button(Input_manager::KEYS::KEY_S));

	gfx::Animator_controller *panim_controller = get_player_anim_controller();

	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////// set up sprites////////////////////////////////////////////////////
	//gfx::Shader sprite_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/sprite.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/sprite.frag");
	string_id sprite_shader_id = g_engine.m_graphics_manager.load_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/sprite.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/sprite.frag");
	
	g_engine.m_graphics_manager.set_current_shader_program(sprite_shader_id);
	gfx::Shader *psprite_shader = g_engine.m_graphics_manager.get_shader(sprite_shader_id);
	
	g_engine.m_graphics_manager.uniform_matrix4fv(psprite_shader->get_uniform_location("V"), 1, false, V.value_ptr());
	g_engine.m_graphics_manager.uniform_matrix4fv(psprite_shader->get_uniform_location("P"), 1, false, camera.projection().value_ptr());
	g_engine.m_graphics_manager.uniform_1f(psprite_shader->get_uniform_location("tileset"), 0);

	/// Player setup
	
	// load atlas needed for the player sprite
	string_id player_atlas_id = g_engine.m_graphics_manager.load_sprite_atlas("C:/Users/mateu/Documents/GitHub/Demos/2d_game/resources/sprite sheets/character.xml");

	// load player's sprite component
	gfx::Sprite *pplayer_sprite = g_engine.m_graphics_manager.get_sprite_component(player_atlas_id, 1);

	math::vec3 player_pos(10.0f, 12.0f);
	
	physics_2d::AABB_2d p_aabb(math::vec2(-0.40f, -0.75f), math::vec2(0.40f, 0.75f));
	math::vec2 pos(player_pos.x, player_pos.y);
	p_aabb.p_max += pos;
	p_aabb.p_min += pos;

	physics_2d::Body_2d *pbody = g_engine.m_physics_manager.get_world()->create_body_2d(physics_2d::Body_2d::Entity_types::DYNAMIC, pos, 1.0f, p_aabb);
	pbody->set_velocity_threshold(math::vec2(6.0f, 12.0f));
	
	Player player(pplayer_sprite, panim_controller, pbody, math::vec3(10.0f, 12.0f), math::mat4(), physics_2d::AABB_2d(), math::vec2(1.5f, 1.0f));
	
	pbody->set_user_data(static_cast<Game_object*>(&player));
	std::cout << "player aabb in world space: max = " << p_aabb.p_max.x << ", " << p_aabb.p_max.y  << " min = " << p_aabb.p_min.x << ", " << p_aabb.p_min.y << std::endl;

	g_engine.m_graphics_manager.set_clear_color(math::vec4(0.0f, 0.0f, 0.5f, 1.0f) );
	g_engine.m_graphics_manager.set_blend_func();
	g_engine.m_graphics_manager.graphics_enable(gfx::GFX_ENUMS::BLEND);

	
	//last_time = glfwGetTime();
	float lag = 0.0f;
	//float elapsed; // is delta
	//g_timer.init();
	
	float bigger_dt = -1;
	float smmalest_dt = 100;
	unsigned c = 0;
	bool on_ground = false;
	
	//set the shaders for sprite and tile map
	g_engine.m_graphics_manager.set_tile_map_shader_id(tile_map_shader_id);
	g_engine.m_graphics_manager.set_sprite_shader_id(sprite_shader_id);
	

	while (!g_engine.m_graphics_manager.window_should_close()) {
		//int i = 0;
		//float physics_update_dt;
		//last_time = curr_time;
		//curr_time = glfwGetTime();
		//elapsed = curr_time - last_time;
		//last_time = curr_time;
		
		//float frame_time = curr_time - last_time;
		std::cout << "FPS: " << g_engine.m_timer.get_fps() << std::endl;

		g_engine.m_timer.update();
		lag += g_engine.m_timer.get_dt();

		float frame_time = g_engine.m_timer.get_dt();
		//std::cout << "delta time shold be " << dt << " | dt = " << frame_time << std::endl;
		
		player.handle_input();
		
		bool  lagging = (frame_time > dt) ? true :false;
		if (lagging) {
			std::cout << "WE ARE BEHIND SCHEDULE by !!!!! " << (frame_time - dt) * 1000.0f << std::endl;
		}
		else {
			std::cout << "WE ARE ON SCHEDULE" << std::endl;
		}

		if (lag >= bigger_dt) {
			bigger_dt = frame_time;
		}

		if (frame_time < smmalest_dt && (c > 0)) {
			smmalest_dt = frame_time;
		}
		else {
			c = 1;
		}
		while (lag >= dt) {
			//++i;
			//float delta_time = (frame_time >= dt) ?dt :frame_time;
			g_engine.m_physics_manager.get_world()->update(g_engine.m_timer.get_fixed_dt());
			
			//std::cout << "frame time = " << frame_time << " | delta_time = " << delta_time << std::endl;
			//frame_time -= delta_time;
			lag -= dt;
			
 		}
		//std::cout <<  "RENDERING AFTER " << i << " world.update() calls" << std::endl;
		player.update();
		
		camera.follow(player.get_body_2d_component()->get_position());
	
		V = camera.get_view();

		g_engine.m_graphics_manager.set_current_shader_program(tile_map_shader_id);
		g_engine.m_graphics_manager.uniform_matrix4fv(v_loc, 1, false, V.value_ptr());
		
		
		g_engine.m_graphics_manager.get_framebuffer_size(&vport_width, &vport_height);
		g_engine.m_graphics_manager.set_viewport(0, 0, vport_width, vport_height);
		
		g_engine.m_graphics_manager.set_current_shader_program(sprite_shader_id);
		g_engine.m_graphics_manager.uniform_matrix4fv(psprite_shader->get_uniform_location("V"), 1, false, V.value_ptr());
		
		g_engine.m_graphics_manager.render();

	}

	std::wcout << "[min dt, max dt] = [" << smmalest_dt * 1000.0f << ", " << bigger_dt * 1000.0f << "]" << std::endl;
	g_engine.shut_down();
	return 0;
}