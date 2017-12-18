#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "SOIL.h"

#include <iostream>
#include <memory>

//cgm
#include "vec3.hpp"
#include "mat4.hpp"
#include "matrix_transform.hpp"

#include "Timer.hpp"

#include "Camera_2d.hpp"
#include "Tile_map.hpp"
#include "Tile_map_renderer.hpp"
#include "Shader.hpp"
#include "tmx_parser.hpp"
#include "Sprite_batch.hpp"

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

#define FRAME_RATE 60.0f

float curr_time;
float last_time;
float dt = 1.0F / FRAME_RATE;

// Engine subsystems singletons
physics_2d::Physics_manager g_physics_manager;
Timer                       g_timer;
Input_manager               g_input_manager;

void error_callback(int error, const char * descr)
{
	std::cerr << "GLFW ERROR: " << descr << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	g_input_manager.key_callback(window, key, scancode, action, mods);
}

std::unique_ptr<Animator_controller> get_player_anim_controller() 
{
	tgs::Animation_player player_idle_anim({ tgs::Animation({ 0, 1 }, 5) });
	tgs::Animation_player player_running_anim({ tgs::Animation({3, 4, 5}, 10) });
	tgs::Animation_player player_jumping_anim({ tgs::Animation({7}, 1) });
	tgs::Animation_player player_climbing_anim({ tgs::Animation({ 8, 9 }, 6),  tgs::Animation({ 10, 11 }, 6) });

	Animation_state player_idle_state("player_idle", player_idle_anim);
	Animation_state player_running_state("player_running", player_running_anim);
	Animation_state player_jumping_state("player_jumping", player_jumping_anim);
	Animation_state player_climbing_state("player_climbing", player_climbing_anim);
	
	std::unique_ptr<Animator_controller> upcontroller(new Animator_controller());
	upcontroller->add_bool_param("is_running");
	upcontroller->add_bool_param("is_jumping");
	upcontroller->add_bool_param("is_climbing");

	upcontroller->add_state(player_idle_state);
	upcontroller->add_state(player_running_state);
	upcontroller->add_state(player_jumping_state);
	upcontroller->add_state(player_climbing_state);

	Transition start_running("start_running", "player_idle", "player_running");
	upcontroller->add_transition(start_running);
	upcontroller->add_bool_condition("start_running", "is_running", true);

	Transition stop_running("stop_running", "player_running", "player_idle");
	upcontroller->add_transition(stop_running);
	upcontroller->add_bool_condition("stop_running", "is_running", false);

	Transition start_jumping_from_idle("start_jumping_from_idle", "player_idle", "player_jumping");
	upcontroller->add_transition(start_jumping_from_idle);
	upcontroller->add_bool_condition("start_jumping_from_idle", "is_jumping", true);

	Transition stop_jumping_to_idle("stop_jumping_to_idle", "player_jumping", "player_idle");
	upcontroller->add_transition(stop_jumping_to_idle);
	upcontroller->add_bool_condition("stop_jumping_to_idle", "is_jumping", false);
	upcontroller->add_bool_condition("stop_jumping_to_idle", "is_running", false);
	upcontroller->add_bool_condition("stop_jumping_to_idle", "is_climbing", false);

	Transition start_jumping_from_running("start_jumping_from_running", "player_running", "player_jumping");
	upcontroller->add_transition(start_jumping_from_running);
	upcontroller->add_bool_condition("start_jumping_from_running", "is_jumping", true);
	//upcontroller->add_booll_condition("start_jumping_from_running", "is_running", true);

	Transition stop_jumping_to_running("stop_jumping_to_running", "player_jumping", "player_running");
	upcontroller->add_transition(stop_jumping_to_running);
	upcontroller->add_bool_condition("stop_jumping_to_running", "is_jumping", false);
	upcontroller->add_bool_condition("stop_jumping_to_running", "is_running", true);
	
	Transition climbing_from_idle("climbing_from_idle", "player_idle", "player_climbing");
	upcontroller->add_transition(climbing_from_idle);
	upcontroller->add_bool_condition("climbing_from_idle", "is_climbing", true);

	//this transition is gonna go, should be from ending climb
	Transition idle_from_climbing("idle_from_climbing", "player_climbing", "player_idle");
	upcontroller->add_transition(idle_from_climbing);
	upcontroller->add_bool_condition("idle_from_climbing", "is_climbing", false);
	upcontroller->add_bool_condition("idle_from_climbing", "is_jumping", false);

	Transition jumping_from_climbing("jumping_from_climbing", "player_climbing", "player_jumping");
	upcontroller->add_transition(jumping_from_climbing);
	upcontroller->add_bool_condition("jumping_from_climbing", "is_climbing", false);
	upcontroller->add_bool_condition("jumping_from_climbing", "is_jumping", true);

	Transition climbing_from_jumping("climbing_from_jumping", "player_jumping", "player_climbing");
	upcontroller->add_transition(climbing_from_jumping);
	upcontroller->add_bool_condition("climbing_from_jumping", "is_climbing", true);
	upcontroller->add_bool_condition("climbing_from_jumping", "is_jumping", false);

	std::cout << (*upcontroller);
	return upcontroller;
}


int main(int argc, char *argv[])
{
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
	}

	
	glfwSetKeyCallback(window, key_callback);

	int vport_width, vport_height;
	glfwGetFramebufferSize(window, &vport_width, &vport_height);

	glViewport(0, 0, vport_width, vport_height);

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
	gls::Shader shader("C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/vertex.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/fragment.frag");

	Tile_map_renderer map_renderer(tile_map, shader);

	cgm::mat4 V;
	GLint v_loc, p_loc;
	GLint sampler_loc;
	
	tgs::Camera_2d camera(1.0f, 1.0f, 16, 15);

	//cgm::vec3 t(-floor(tile_map.width() / (float)2), -floor(tile_map.height() / (float)2), 0.0f);
    //V = cgm::rotate_y(180.0f);
	//V.transpose();
	//V = cgm::mat4(cgm::mat4(), t);

	//P = cgm::ortho(-floor(tile_map.width() / (float)2),  ceil(tile_map.width() / (float)2), -floor(tile_map.height() / (float)2), ceil(tile_map.height() / (float)2), -1.0, 1.0f);
	cgm::vec3 p = camera.get_position();
	p.x = -p.x;
	p.y = -p.y;
	V = cgm::mat4(cgm::mat4(), p);

	v_loc = shader.get_uniform_location("V");
	p_loc = shader.get_uniform_location("P");
	sampler_loc = shader.get_uniform_location("tileset1");

	glUniformMatrix4fv(v_loc, 1, GL_FALSE, V.value_ptr());
	glUniformMatrix4fv(p_loc, 1, GL_FALSE, camera.projection().value_ptr());
	glUniform1f(sampler_loc, 0);

	///////////////////--------------------------Initialization code------------------ ///////////////////////////////////////////////////////////////////////////
	//initialize physics engine
	Game_coll_listener game_coll_listener;
	g_physics_manager.init(&tile_map);
	g_physics_manager.get_world()->set_collision_listener(&game_coll_listener);
	//Initialize timmer
	g_timer.init();
	// Initialize Input_manager
	g_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::MOVE_LEFT, Button(Input_manager::KEYS::KEY_LEFT));
	g_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::JUMP, Button(Input_manager::KEYS::KEY_S));
	g_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::MOVE_RIGHT, Button(Input_manager::KEYS::KEY_RIGHT));
	g_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::CLIMB_UP, Button(Input_manager::KEYS::KEY_UP));
	g_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::CLIMB_DOWN, Button(Input_manager::KEYS::KEY_DOWN));
	g_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::MOVE_UP, Button(Input_manager::KEYS::KEY_UP));
	g_input_manager.map_action_to_button(Input_manager::GAME_ACTIONS::MOVE_DOWN, Button(Input_manager::KEYS::KEY_DOWN));
	
	//Player_idle_state::switch_anim_frames({ 0, 1 });
	//Player_running_state::switch_anim_frames({ 3, 4, 5 });

	std::unique_ptr<Animator_controller> upanim_controller = get_player_anim_controller();

	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////// set up sprites////////////////////////////////////////////////////
	gls::Shader sprite_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/sprite.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/sprite.frag");
	sprite_shader.use();
	glUniformMatrix4fv(sprite_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
	glUniformMatrix4fv(sprite_shader.get_uniform_location("P"), 1, GL_FALSE, camera.projection().value_ptr());
	glUniform1f(sprite_shader.get_uniform_location("tileset"), 0);

	/// Player setup
	Player player(cgm::vec3(10.0f, 12.0f), cgm::mat4(), AABB_2d() ,cgm::vec2(1.5f, 1.0f));
	
	AABB_2d p_aabb(cgm::vec2(-0.40f, -0.75f), cgm::vec2(0.40f, 0.75f));
	cgm::vec2 pos(player.get_position().x, player.get_position().y);
	p_aabb.p_max += pos;
	p_aabb.p_min += pos;
	physics_2d::Body_2d *pbody = g_physics_manager.get_world()->create_body_2d(physics_2d::Body_2d::Entity_types::DYNAMIC, pos, 1.0f, p_aabb);
	
	pbody->set_velocity_threshold(cgm::vec2(6.0f, 12.0f));
	player.set_body_2d(pbody);
	pbody->set_user_data(static_cast<Game_object*>(&player));
	std::cout << "player aabb in world space: max = " << p_aabb.p_max.x << ", " << p_aabb.p_max.y  << " min = " << p_aabb.p_min.x << ", " << p_aabb.p_min.y << std::endl;

	player.get_sprite().set_anim_controller(upanim_controller);
	/////////////////////////////////////////////////////////////////////
	tgs::Sprite_batch batch(12, player.get_sprite().get_atlas()->get_texture() , sprite_shader);
	batch.add(player.get_sprite());
	
	/////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////// MAP DEBUG SHADERS ////////////////////////////////////////////////////////////////////////
		gls::Shader display_grid_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/d_display_tile_grid.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/d_display_tile_grid.frag");
		display_grid_shader.use();
		glUniformMatrix4fv(display_grid_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
		glUniformMatrix4fv(display_grid_shader.get_uniform_location("P"), 1, GL_FALSE, camera.projection().value_ptr());
		map_renderer.set_debug_mode(Tile_map_renderer::Debug_options::DISPLAY_GRID, display_grid_shader);

		gls::Shader display_colliders_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/d_display_colliders.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/d_display_colliders.frag");
		display_colliders_shader.use();
		glUniformMatrix4fv(display_colliders_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
		glUniformMatrix4fv(display_colliders_shader.get_uniform_location("P"), 1, GL_FALSE, camera.projection().value_ptr());
		
		map_renderer.set_debug_mode(Tile_map_renderer::Debug_options::DISPLAY_COLLIDERS, display_colliders_shader);
	/////////////////////////////////////////////////////////////////////////////////////////

	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	//last_time = glfwGetTime();
	float lag = 0.0f;
	//float elapsed; // is delta
	//g_timer.init();
	
	float bigger_dt = -1;
	float smmalest_dt = 100;
	unsigned c = 0;
	bool on_ground = false;
	while (!glfwWindowShouldClose(window)) {
		//int i = 0;
		//float physics_update_dt;
		//last_time = curr_time;
		//curr_time = glfwGetTime();
		//elapsed = curr_time - last_time;
		//last_time = curr_time;
		
		//float frame_time = curr_time - last_time;
		std::cout << "FPS: " << g_timer.get_fps() << std::endl;

		g_timer.update();
		lag += g_timer.get_dt();

		float frame_time = g_timer.get_dt();
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
			g_physics_manager.get_world()->update();
			
			//std::cout << "frame time = " << frame_time << " | delta_time = " << delta_time << std::endl;
			//frame_time -= delta_time;
			lag -= dt;
			
 		}
		//std::cout <<  "RENDERING AFTER " << i << " world.update() calls" << std::endl;
		player.update();
		
		camera.follow(player.get_body_2d()->get_position());
	
		V = camera.get_view();

		shader.use();
		glUniformMatrix4fv(v_loc, 1, GL_FALSE, V.value_ptr());
		
		display_grid_shader.use();
		glUniformMatrix4fv(display_grid_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());

		display_colliders_shader.use();
		glUniformMatrix4fv(display_colliders_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());

		//std::cout << "PLAYER WLD SPACE POS = (" << player.get_body_2d()->get_position().x << ", " << player.get_body_2d()->get_position().y << ")" << std::endl;
		glfwGetFramebufferSize(window, &vport_width, &vport_height);

		glViewport(0, 0, vport_width, vport_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		map_renderer.render(shader);
		
		sprite_shader.use();
		glUniformMatrix4fv(sprite_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
		batch.render();
		glBindTexture(GL_TEXTURE_2D, 0);
		glfwPollEvents();
		glfwSwapBuffers(window);

		batch.add(player.get_sprite());
	}

	std::wcout << "[min dt, max dt] = [" << smmalest_dt * 1000.0f << ", " << bigger_dt * 1000.0f << "]" << std::endl;

	g_physics_manager.shut_down();
	glfwTerminate();
	return 0;
}