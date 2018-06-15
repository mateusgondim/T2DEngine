
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
#include "Body_2d_def.hpp"
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
#include "Shader_manager.hpp"
#include "Texture_2d_manager.hpp"
#include "Sprite_atlas.hpp"
#include "Sprite_atlas_manager.hpp"
#include "Graphics_manager.hpp"

//gom
#include "Game_object_handle.hpp"
#include "Game_object_manager.hpp"

#include "Player_creator.hpp"
#include "Projectile_creator.hpp"

//memory
#include "runtime_memory_allocator.hpp"

//utility
#include "string_id.hpp"
#include "Path.hpp"

#include <iostream>

#include "engine.hpp"



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
	io::key_callback(key, scancode, action, mods);
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
	Path resources_path("../resources", Path::FORWARD_SLASH);
	// Load tile map
	//$(OutDir);
	Tile_map tile_map;

	if (!load_tile_map( (resources_path + "/maps/test_map.tmx").c_str(), tile_map)) {
		std::cout << "Tile_map was loaded " << std::endl;
		print_tile_map(std::cout, tile_map);
	}
	else {
		std::cout << "ERROR: Could not load Tile_map " << std::endl;
		return -1;
	}
	engine_init(4, 3, &tile_map);
	
	gfx::g_graphics_mgr.set_error_callback(error_callback);
	gfx::g_graphics_mgr.create_window(512, 480, "2D Game");
	gfx::g_graphics_mgr.set_key_callback(key_callback);

	int vport_width, vport_height;

	gfx::g_graphics_mgr.get_framebuffer_size(&vport_width, &vport_height);

	gfx::g_graphics_mgr.set_viewport(0, 0, vport_width, vport_height);

	
	// CHANGE ABSOLUTE PATH!!!!!!!
	gfx::Shader   *ptile_map_shader = static_cast<gfx::Shader*>(gfx::g_shader_mgr.load("tile_map_shader", (resources_path + "/shaders/vertex.vert").c_str(), (resources_path + "/shaders/fragment.frag").c_str() ));

	//load the data to render the map into the graphics manager
	gfx::g_graphics_mgr.set_tile_map(&tile_map);

	math::mat4 V;
	
	int32_t v_loc, p_loc;
	int32_t sampler_loc;

	//gfx::Camera_2d camera(1.0f, 1.0f, 16, 15, tile_map.width(), tile_map.height());

	//math::vec3 p = camera.get_position();
	//p.x = -p.x;
	//p.y = -p.y;
	V = gfx::g_graphics_mgr.get_camera().get_view();

	v_loc = ptile_map_shader->get_uniform_location("V");
	p_loc = ptile_map_shader->get_uniform_location("P");
	sampler_loc = ptile_map_shader->get_uniform_location("tileset1");

	//set uniforms on current shader program
	ptile_map_shader->use();
	gfx::g_graphics_mgr.uniform_matrix4fv(v_loc, 1, false, V.value_ptr());
	gfx::g_graphics_mgr.uniform_matrix4fv(p_loc, 1, false, gfx::g_graphics_mgr.get_camera().projection().value_ptr());
	gfx::g_graphics_mgr.uniform_1f(sampler_loc, 0);

	///////////////////--------------------------Initialization code------------------ ///////////////////////////////////////////////////////////////////////////
	//Set physics engine collision listener
	Game_coll_listener game_coll_listener;
	physics_2d::g_physics_mgr.get_world()->set_collision_listener(&game_coll_listener);
	
	// Initialize Input_manager
	io::map_action_to_button(io::GAME_ACTIONS::MOVE_LEFT, Button(io::KEYS::KEY_LEFT));
	io::map_action_to_button(io::GAME_ACTIONS::JUMP, Button(io::KEYS::KEY_A));
	io::map_action_to_button(io::GAME_ACTIONS::MOVE_RIGHT, Button(io::KEYS::KEY_RIGHT));
	io::map_action_to_button(io::GAME_ACTIONS::CLIMB_UP, Button(io::KEYS::KEY_UP));
	io::map_action_to_button(io::GAME_ACTIONS::CLIMB_DOWN, Button(io::KEYS::KEY_DOWN));
	io::map_action_to_button(io::GAME_ACTIONS::MOVE_UP, Button(io::KEYS::KEY_UP));
	io::map_action_to_button(io::GAME_ACTIONS::MOVE_DOWN, Button(io::KEYS::KEY_DOWN));
	io::map_action_to_button(io::GAME_ACTIONS::ATTACK_01, Button(io::KEYS::KEY_S));

	//gfx::Animator_controller *panim_controller = get_player_anim_controller();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////// set up sprites////////////////////////////////////////////////////
	//gfx::Shader sprite_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/sprite.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/sprite.frag");
	//string_id sprite_shader_id = g_systems.m_graphics_manager.load_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/sprite.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/sprite.frag");
	gfx::Shader *psprite_shader = static_cast<gfx::Shader*>(gfx::g_shader_mgr.load("sprite_shader", (resources_path + "/shaders/sprite.vert").c_str(), (resources_path + "/shaders/sprite.frag").c_str() ));

	psprite_shader->use();
	gfx::g_graphics_mgr.uniform_matrix4fv(psprite_shader->get_uniform_location("V"), 1, false, V.value_ptr());
	gfx::g_graphics_mgr.uniform_matrix4fv(psprite_shader->get_uniform_location("P"), 1, false, gfx::g_graphics_mgr.get_camera().projection().value_ptr());
	gfx::g_graphics_mgr.uniform_1f(psprite_shader->get_uniform_location("tileset"), 0);

	/// Player setup
	std::cout << sizeof(void*) << std::endl;

	// load atlas needed for the player sprite
	gfx::Sprite_atlas *patlas = static_cast<gfx::Sprite_atlas*>(gfx::g_sprite_atlas_mgr.load("player", (resources_path + "/sprite sheets/character.xml").c_str() ,&gfx::g_texture_2d_mgr));
	
	//get the atlas resource id
	string_id atlas_id = patlas->get_id();
	
	// Create the Player Creator class
	Player_creator *pplayer_creator = new Player_creator(atlas_id, 0);

	//create a type id for the player object. THIS SHOULD BE READ FROM A FILE
	string_id player_type_id = intern_string("Player_object");

	//register the creator. CAREFULL PASSING UINT32_T , SHOULD BE A UINT16_T, FIX IT!
	gom::g_game_object_mgr.register_creator(player_type_id, pplayer_creator);

	//Create the player object
	gom::Game_object_handle player_handle = gom::g_game_object_mgr.instantiate(player_type_id, math::vec3());
	Player *pplayer = static_cast<Player*>(gom::g_game_object_mgr.get_by_handle(player_handle));
	
	// set the player projectile creator
	physics_2d::Body_2d_def body_def;
	body_def.m_position = math::vec2();
	body_def.m_aabb = physics_2d::AABB_2d(math::vec2(-0.20f, -0.1f), math::vec2(0.20f, 0.1f));
	body_def.m_type = physics_2d::Body_2d::DYNAMIC;
	body_def.m_velocity = math::vec2(1.0f, 0.0f);

	// set the player projectile anim_controller
	gfx::Animator_controller *pcontroller(new gfx::Animator_controller());
	gfx::Animation_player knife_idle_anim(gfx::Animation({ 7 }, 5));
	pcontroller->add_state("knife_idle_state", knife_idle_anim);

	//create a type id for the object
	string_id knife_type_id = intern_string("knife_obj");
	//creator
	Projectile_creator *knife_projectile = new Projectile_creator(atlas_id, body_def, pcontroller);

	gom::g_game_object_mgr.register_creator(knife_type_id, knife_projectile);

	gfx::g_graphics_mgr.set_clear_color(math::vec4(0.0f, 0.0f, 0.5f, 1.0f) );
	gfx::g_graphics_mgr.set_blend_func();
	gfx::g_graphics_mgr.graphics_enable(gfx::GFX_ENUMS::BLEND);

	
	//last_time = glfwGetTime();
	float lag = 0.0f;
	//float elapsed; // is delta
	//g_timer.init();
	
	float bigger_dt = -1;
	float smmalest_dt = 100;
	unsigned c = 0;
	bool on_ground = false;
	
	//set the shaders for sprite and tile map
	gfx::g_graphics_mgr.set_tile_map_shader(ptile_map_shader);
	gfx::g_graphics_mgr.set_sprite_shader(psprite_shader);
	
	Timer timer;
	timer.init();

	auto coord = tile_map.wld_to_tile_space(math::vec3(10.0f, 12.0f));

	std::cout << " TILE MAP WIDTH = " << tile_map.width() << "| TILE MAP HEIGHT = " << tile_map.height() << std::endl;

	while (!gfx::g_graphics_mgr.window_should_close()) {
		
		//int i = 0;
		//float physics_update_dt;
		//last_time = curr_time;
		//curr_time = glfwGetTime();
		//elapsed = curr_time - last_time;
		//last_time = curr_time;
		
		//float frame_time = curr_time - last_time;
		std::cout << "FPS: " << timer.get_fps() << std::endl;

		timer.update();
		lag += timer.get_dt();

		float frame_time = timer.get_dt();
		//std::cout << "delta time shold be " << dt << " | dt = " << frame_time << std::endl;
		
		pplayer->handle_input();
		
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
			physics_2d::g_physics_mgr.get_world()->update(timer.get_fixed_dt());
			
			//std::cout << "frame time = " << frame_time << " | delta_time = " << delta_time << std::endl;
			//frame_time -= delta_time;
			lag -= dt;
			
 		}
		//std::cout <<  "RENDERING AFTER " << i << " world.update() calls" << std::endl;
		//pplayer->update(timer.get_dt());
		gom::g_game_object_mgr.update_game_objects(timer.get_dt());

		gfx::g_graphics_mgr.get_camera().follow(pplayer->get_body_2d_component()->get_position());
	
		V = gfx::g_graphics_mgr.get_camera().get_view();

		ptile_map_shader->use();
		gfx::g_graphics_mgr.uniform_matrix4fv(v_loc, 1, false, V.value_ptr());
		
		
		gfx::g_graphics_mgr.get_framebuffer_size(&vport_width, &vport_height);
		gfx::g_graphics_mgr.set_viewport(0, 0, vport_width, vport_height);
		
		psprite_shader->use();
		gfx::g_graphics_mgr.uniform_matrix4fv(psprite_shader->get_uniform_location("V"), 1, false, V.value_ptr());
		
		gfx::g_graphics_mgr.render();

	}

	std::wcout << "[min dt, max dt] = [" << smmalest_dt * 1000.0f << ", " << bigger_dt * 1000.0f << "]" << std::endl;
	//gom::g_game_object_mgr.destroy(player_handle);
	engine_shut_down();
	return 0;
}