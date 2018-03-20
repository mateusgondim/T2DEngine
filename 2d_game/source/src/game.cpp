
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
#include "Animation_state.hpp"
#include "Transition.hpp"
#include "Animator_controller.hpp"
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
	gfx::Animation_player player_idle_anim({ gfx::Animation({5}, 5) });
	gfx::Animation_player player_running_anim({ gfx::Animation({8, 10, 9}, 10) });
	gfx::Animation_player player_jumping_anim({ gfx::Animation({6}, 1) });
	gfx::Animation_player player_climbing_anim({ gfx::Animation({ 1, 2 }, 6),  gfx::Animation({3 }, 6) });
	gfx::Animation_player player_attacking_anim({ gfx::Animation({ 13, 14 }, 10, false) });
	gfx::Animation_player player_ducking_idle_anim({ gfx::Animation({4}, 1) });
	gfx::Animation_player player_ducking_attacking_anim({ gfx::Animation({ 11, 12 }, 10, false) });

	gfx::Animation_state player_idle_state("player_idle", player_idle_anim);
	gfx::Animation_state player_running_state("player_running", player_running_anim);
	gfx::Animation_state player_jumping_state("player_jumping", player_jumping_anim);
	gfx::Animation_state player_climbing_state("player_climbing", player_climbing_anim);
	gfx::Animation_state player_attacking_state("player_attacking", player_attacking_anim);
	gfx::Animation_state player_ducking_idle_state("player_ducking_idle", player_ducking_idle_anim);
	gfx::Animation_state player_ducking_attacking_state("player_ducking_attacking", player_ducking_attacking_anim);

	gfx::Animator_controller *pcontroller(new gfx::Animator_controller());
	pcontroller->add_bool_param("is_running");
	pcontroller->add_bool_param("is_jumping");
	pcontroller->add_bool_param("is_climbing");
	pcontroller->add_bool_param("is_ducking");
	pcontroller->add_trigger_param("is_attacking");

	pcontroller->add_state(player_idle_state);
	pcontroller->add_state(player_running_state);
	pcontroller->add_state(player_jumping_state);
	pcontroller->add_state(player_climbing_state);
	pcontroller->add_state(player_attacking_state);
	pcontroller->add_state(player_ducking_idle_state);
	pcontroller->add_state(player_ducking_attacking_state);

	gfx::Transition idle_to_ducking_idle("idle_to_ducking_idle", "player_idle", "player_ducking_idle");
	pcontroller->add_transition(idle_to_ducking_idle);
	pcontroller->add_bool_condition("idle_to_ducking_idle", "is_ducking", true);

	gfx::Transition ducking_idle_to_idle("ducking_idle_to_idle", "player_ducking_idle", "player_idle");
	pcontroller->add_transition(ducking_idle_to_idle);
	pcontroller->add_bool_condition("ducking_idle_to_idle", "is_ducking", false);

	gfx::Transition ducking_idle_to_ducking_attacking("ducking_idle_to_ducking_attacking", "player_ducking_idle", "player_ducking_attacking");
	pcontroller->add_transition(ducking_idle_to_ducking_attacking);
	pcontroller->add_trigger_condition("ducking_idle_to_ducking_attacking", "is_attacking", true);

	gfx::Transition ducking_attacking_to_ducking_idle("ducking_attacking_to_ducking_idle", "player_ducking_attacking", "player_ducking_idle");
	pcontroller->add_transition(ducking_attacking_to_ducking_idle);
	pcontroller->add_trigger_condition("ducking_attacking_to_ducking_idle", "is_attacking", false);


	gfx::Transition attack_from_idle("idle_to_attack", "player_idle", "player_attacking");
	pcontroller->add_transition(attack_from_idle);
	pcontroller->add_trigger_condition("idle_to_attack", "is_attacking", true);

	gfx::Transition idle_from_attacking("attack_to_idle", "player_attacking", "player_idle");
	pcontroller->add_transition(idle_from_attacking);
	pcontroller->add_trigger_condition("attack_to_idle", "is_attacking", false);
	pcontroller->add_bool_condition("attack_to_idle", "is_jumping", false);

	gfx::Transition running_to_attack("running_to_attack", "player_running", "player_attacking");
	pcontroller->add_transition(running_to_attack);
	pcontroller->add_trigger_condition("running_to_attack", "is_attacking", true);

	
	gfx::Transition jumping_to_attack("jumping_to_attack", "player_jumping", "player_attacking");
	pcontroller->add_transition(jumping_to_attack);
	pcontroller->add_trigger_condition("jumping_to_attack", "is_attacking", true);

	gfx::Transition attack_to_jumping("attack_to_jumping", "player_attacking", "player_jumping");
	pcontroller->add_transition(attack_to_jumping);
	pcontroller->add_trigger_condition("attack_to_jumping", "is_attacking", false);
	pcontroller->add_bool_condition("attack_to_jumping", "is_jumping", true);


	gfx::Transition start_running("start_running", "player_idle", "player_running");
	pcontroller->add_transition(start_running);
	pcontroller->add_bool_condition("start_running", "is_running", true);

	gfx::Transition stop_running("stop_running", "player_running", "player_idle");
	pcontroller->add_transition(stop_running);
	pcontroller->add_bool_condition("stop_running", "is_running", false);
	pcontroller->add_trigger_condition("stop_running", "is_attacking", false);

	gfx::Transition start_jumping_from_idle("start_jumping_from_idle", "player_idle", "player_jumping");
	pcontroller->add_transition(start_jumping_from_idle);
	pcontroller->add_bool_condition("start_jumping_from_idle", "is_jumping", true);

	gfx::Transition stop_jumping_to_idle("stop_jumping_to_idle", "player_jumping", "player_idle");
	pcontroller->add_transition(stop_jumping_to_idle);
	pcontroller->add_bool_condition("stop_jumping_to_idle", "is_jumping", false);
	pcontroller->add_bool_condition("stop_jumping_to_idle", "is_running", false);
	pcontroller->add_bool_condition("stop_jumping_to_idle", "is_climbing", false);

	gfx::Transition start_jumping_from_running("start_jumping_from_running", "player_running", "player_jumping");
	pcontroller->add_transition(start_jumping_from_running);
	pcontroller->add_bool_condition("start_jumping_from_running", "is_jumping", true);
	//upcontroller->add_booll_condition("start_jumping_from_running", "is_running", true);

	gfx::Transition stop_jumping_to_running("stop_jumping_to_running", "player_jumping", "player_running");
	pcontroller->add_transition(stop_jumping_to_running);
	pcontroller->add_bool_condition("stop_jumping_to_running", "is_jumping", false);
	pcontroller->add_bool_condition("stop_jumping_to_running", "is_running", true);
	
	gfx::Transition climbing_from_idle("climbing_from_idle", "player_idle", "player_climbing");
	pcontroller->add_transition(climbing_from_idle);
	pcontroller->add_bool_condition("climbing_from_idle", "is_climbing", true);

	//this transition is gonna go, should be from ending climb
	gfx::Transition idle_from_climbing("idle_from_climbing", "player_climbing", "player_idle");
	pcontroller->add_transition(idle_from_climbing);
	pcontroller->add_bool_condition("idle_from_climbing", "is_climbing", false);
	pcontroller->add_bool_condition("idle_from_climbing", "is_jumping", false);

	gfx::Transition jumping_from_climbing("jumping_from_climbing", "player_climbing", "player_jumping");
	pcontroller->add_transition(jumping_from_climbing);
	pcontroller->add_bool_condition("jumping_from_climbing", "is_climbing", false);
	pcontroller->add_bool_condition("jumping_from_climbing", "is_jumping", true);

	gfx::Transition climbing_from_jumping("climbing_from_jumping", "player_jumping", "player_climbing");
	pcontroller->add_transition(climbing_from_jumping);
	pcontroller->add_bool_condition("climbing_from_jumping", "is_climbing", true);
	pcontroller->add_bool_condition("climbing_from_jumping", "is_jumping", false);

	std::cout << (*pcontroller);
	return pcontroller;
}


int main(int argc, char *argv[])
{
	/*
	glfwSetErrorCallback(error_callback);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * window = glfwCreateWindow(512, 480, "2D Game", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);


	glewExperimental = GL_TRUE; // allow to use all of opengl core profile

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}*/
	g_engine.m_graphics_manager.set_error_callback(error_callback);
	g_engine.m_graphics_manager.init(4, 3);
	g_engine.m_graphics_manager.create_window(512, 480, "2D Game");

	
	//glfwSetKeyCallback(window, key_callback);
	g_engine.m_graphics_manager.set_key_callback(key_callback);

	int vport_width, vport_height;

	//glfwGetFramebufferSize(window, &vport_width, &vport_height);
	g_engine.m_graphics_manager.get_framebuffer_size(&vport_width, &vport_height);

	//glViewport(0, 0, vport_width, vport_height);
	g_engine.m_graphics_manager.set_viewport(0, 0, vport_width, vport_height);

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
	// CHANGE ABSOLUTE PATH!!!!!!!
	
	//gfx::Shader shader("C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/vertex.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/fragment.frag");
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

	//glUniformMatrix4fv(v_loc, 1, GL_FALSE, V.value_ptr());
	g_engine.m_graphics_manager.uniform_matrix4fv(v_loc, 1, false, V.value_ptr());

	//glUniformMatrix4fv(p_loc, 1, GL_FALSE, camera.projection().value_ptr());
	g_engine.m_graphics_manager.uniform_matrix4fv(p_loc, 1, false, camera.projection().value_ptr());

	//glUniform1f(sampler_loc, 0);
	g_engine.m_graphics_manager.uniform_1f(sampler_loc, 0);

	g_engine.init(); //timer init
	///////////////////--------------------------Initialization code------------------ ///////////////////////////////////////////////////////////////////////////
	//initialize physics engine
	Game_coll_listener game_coll_listener;
	g_engine.m_physics_manager.init(&tile_map);
	//g_physics_manager.init(&tile_map);

	g_engine.m_physics_manager.get_world()->set_collision_listener(&game_coll_listener);
	//g_physics_manager.get_world()->set_collision_listener(&game_coll_listener);
	
	//Initialize timmer
	//g_timer.init();
	
	// Initialize Input_manager
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::MOVE_LEFT, Button(Input_manager::KEYS::KEY_LEFT));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::JUMP, Button(Input_manager::KEYS::KEY_A));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::MOVE_RIGHT, Button(Input_manager::KEYS::KEY_RIGHT));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::CLIMB_UP, Button(Input_manager::KEYS::KEY_UP));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::CLIMB_DOWN, Button(Input_manager::KEYS::KEY_DOWN));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::MOVE_UP, Button(Input_manager::KEYS::KEY_UP));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::MOVE_DOWN, Button(Input_manager::KEYS::KEY_DOWN));
	g_engine.m_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::ATTACK_01, Button(Input_manager::KEYS::KEY_S));
	
	//Player_idle_state::switch_anim_frames({ 0, 1 });
	//Player_running_state::switch_anim_frames({ 3, 4, 5 });

	gfx::Animator_controller *panim_controller = get_player_anim_controller();

	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////// set up sprites////////////////////////////////////////////////////
	//gfx::Shader sprite_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/sprite.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/sprite.frag");
	string_id sprite_shader_id = g_engine.m_graphics_manager.load_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/sprite.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/sprite.frag");
	

	//sprite_shader.use();
	g_engine.m_graphics_manager.set_current_shader_program(sprite_shader_id);

	gfx::Shader *psprite_shader = g_engine.m_graphics_manager.get_shader(sprite_shader_id);
	

	//glUniformMatrix4fv(sprite_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
	g_engine.m_graphics_manager.uniform_matrix4fv(psprite_shader->get_uniform_location("V"), 1, false, V.value_ptr());


	//glUniformMatrix4fv(sprite_shader.get_uniform_location("P"), 1, GL_FALSE, camera.projection().value_ptr());
	g_engine.m_graphics_manager.uniform_matrix4fv(psprite_shader->get_uniform_location("P"), 1, false, camera.projection().value_ptr());
	
	//glUniform1f(sprite_shader.get_uniform_location("tileset"), 0);
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

	/////////////////////////////////////////////////////////////////////
	//gfx::Sprite_batch batch(12, player.get_sprite_component()->get_atlas()->get_texture());
	//batch.add(*player.get_sprite_component());
	
	/////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////// MAP DEBUG SHADERS ////////////////////////////////////////////////////////////////////////
		//string_id   tile_map_grid_shader_id = g_engine.m_graphics_manager.load_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/d_display_tile_grid.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/d_display_tile_grid.frag");
		//gfx::Shader display_grid_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/d_display_tile_grid.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/d_display_tile_grid.frag");
		
		//g_engine.m_graphics_manager.set_current_shader_program(tile_map_grid_shader_id);
		//display_grid_shader.use();
		
		//gfx::Shader *ptile_map_grid_shader = g_engine.m_graphics_manager.get_shader(tile_map_grid_shader_id);
		
		//glUniformMatrix4fv(display_grid_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
		//g_engine.m_graphics_manager.uniform_matrix4fv(ptile_map_grid_shader->get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());

		//glUniformMatrix4fv(display_grid_shader.get_uniform_location("P"), 1, GL_FALSE, camera.projection().value_ptr());
		//g_engine.m_graphics_manager.uniform_matrix4fv(ptile_map_grid_shader->get_uniform_location("P"), 1, GL_FALSE, camera.projection().value_ptr());

		//map_renderer.set_debug_mode(Tile_map_renderer::Debug_options::DISPLAY_GRID, *ptile_map_grid_shader);

		//string_id   tile_map_coll_shader_id = g_engine.m_graphics_manager.load_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/d_display_colliders.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/d_display_colliders.frag");
		//gfx::Shader display_colliders_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/d_display_colliders.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_game/source/shaders/d_display_colliders.frag");
		
		//g_engine.m_graphics_manager.set_current_shader_program(tile_map_coll_shader_id);
		//display_colliders_shader.use();
		
		//gfx::Shader *ptile_map_coll_shader = g_engine.m_graphics_manager.get_shader(tile_map_coll_shader_id);

		//g_engine.m_graphics_manager.uniform_matrix4fv(ptile_map_coll_shader->get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
		//glUniformMatrix4fv(display_colliders_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());

		//g_engine.m_graphics_manager.uniform_matrix4fv(ptile_map_coll_shader->get_uniform_location("P"), 1, GL_FALSE, camera.projection().value_ptr());
		//glUniformMatrix4fv(display_colliders_shader.get_uniform_location("P"), 1, GL_FALSE, camera.projection().value_ptr());
		
		//map_renderer.set_debug_mode(Tile_map_renderer::Debug_options::DISPLAY_COLLIDERS, *ptile_map_coll_shader);
	/////////////////////////////////////////////////////////////////////////////////////////

	g_engine.m_graphics_manager.set_clear_color(math::vec4(0.0f, 0.0f, 0.5f, 1.0f) );
	//glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	
	g_engine.m_graphics_manager.set_blend_func();
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	g_engine.m_graphics_manager.graphics_enable(gfx::GFX_ENUMS::BLEND);
	//glEnable(GL_BLEND);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	//last_time = glfwGetTime();
	float lag = 0.0f;
	//float elapsed; // is delta
	//g_timer.init();
	
	float bigger_dt = -1;
	float smmalest_dt = 100;
	unsigned c = 0;
	bool on_ground = false;
	//math::Rect bounds;
	//float pixel_width, pixel_height;
	//player.get_sprite_component()->get_atlas()->get_text_coord(13, &bounds, &pixel_width, &pixel_height);
	//std::cout << "TEXTURE COORDINATES X = " << bounds.x << ", Y = " << bounds.y << ", WIDTH = " << bounds.width << ", HEIGHT = " << bounds.height << ", IM_HEIGHT = " << pixel_height<< std::endl;

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
		//shader.use();
		g_engine.m_graphics_manager.uniform_matrix4fv(v_loc, 1, false, V.value_ptr());
		//glUniformMatrix4fv(v_loc, 1, GL_FALSE, V.value_ptr());
		
		//g_engine.m_graphics_manager.set_current_shader_program(tile_map_grid_shader_id);
		//display_grid_shader.use();
		
		//g_engine.m_graphics_manager.uniform_matrix4fv(ptile_map_grid_shader->get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
		//glUniformMatrix4fv(display_grid_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());

		//g_engine.m_graphics_manager.set_current_shader_program(tile_map_coll_shader_id);
		//display_colliders_shader.use();
		
		//g_engine.m_graphics_manager.uniform_matrix4fv(ptile_map_coll_shader->get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
		//glUniformMatrix4fv(display_colliders_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());

		//std::cout << "PLAYER WLD SPACE POS = (" << player.get_body_2d()->get_position().x << ", " << player.get_body_2d()->get_position().y << ")" << std::endl;
		
		g_engine.m_graphics_manager.get_framebuffer_size(&vport_width, &vport_height);
		//glfwGetFramebufferSize(window, &vport_width, &vport_height);

		g_engine.m_graphics_manager.set_viewport(0, 0, vport_width, vport_height);
		//glViewport(0, 0, vport_width, vport_height);
		

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//map_renderer.render(*ptile_map_shader);
		
		
		//sprite_shader.use();
		g_engine.m_graphics_manager.set_current_shader_program(sprite_shader_id);

		g_engine.m_graphics_manager.uniform_matrix4fv(psprite_shader->get_uniform_location("V"), 1, false, V.value_ptr());
		//glUniformMatrix4fv(sprite_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
		
		g_engine.m_graphics_manager.render();

		//batch.render();
		//glBindTexture(GL_TEXTURE_2D, 0);
		//glfwPollEvents();
		//glfwSwapBuffers(window);

		//batch.add(*player.get_sprite_component());
	}

	std::wcout << "[min dt, max dt] = [" << smmalest_dt * 1000.0f << ", " << bigger_dt * 1000.0f << "]" << std::endl;

	g_engine.m_physics_manager.shut_down();
	g_engine.m_graphics_manager.shut_down();
	return 0;
}