#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "SOIL.h"

#include <iostream>
#include <memory>
#include "Timer.hpp"
#include "Tile_map.hpp"
#include "Tile_map_renderer.hpp"
#include "Shader.hpp"
#include "matrix_transform.hpp"
#include "tmx_parser.hpp"
#include "vec3.hpp"
#include "mat4.hpp"
#include "Actor.hpp"
#include "Vertex1P1C1UV.hpp"
#include "Sprite_batch.hpp"
#include "Sprite.hpp"
#include "Input_handler.hpp"
#include "Button.hpp"
#include "Move_left_command.hpp"
#include "Move_right_command.hpp"
#include "Move_down_command.hpp"
#include "Move_up_command.hpp"
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

void error_callback(int error, const char * descr)
{
	std::cerr << "GLFW ERROR: " << descr << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Input_handler::instance().key_callback(window, key, scancode, action, mods);
}

std::unique_ptr<Animator_controller> get_player_anim_controller() 
{
	tgs::Animation_player player_idle_anim({ tgs::Animation({ 0, 1 }, 5) });
	tgs::Animation_player player_running_anim({ tgs::Animation({3, 4, 5}, 8) });

	Animation_state player_idle_state("player_idle", player_idle_anim);
	Animation_state player_running_state("player_running", player_running_anim);
	
	std::unique_ptr<Animator_controller> upcontroller(new Animator_controller());
	upcontroller->add_bool_param("is_running");

	upcontroller->add_state(player_idle_state);
	upcontroller->add_state(player_running_state);
	
	Transition start_running("start_running", "player_idle", "player_running");
	upcontroller->add_transition(start_running);
	upcontroller->add_bool_condition("start_running", "is_running", true);

	Transition stop_running("stop_running", "player_running", "player_idle");
	upcontroller->add_transition(stop_running);
	upcontroller->add_bool_condition("stop_running", "is_running", false);
	
	

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
	Move_left_command	m_left_command;
	Move_right_command	m_right_command;
	Move_down_command	m_down_command;
	Move_up_command		m_up_command;
	

	Input_handler::instance().add_button_n_cmd(Button(KEY_A), &m_left_command);
	Input_handler::instance().add_button_n_cmd(Button(KEY_D), &m_right_command);
	Input_handler::instance().add_button_n_cmd(Button(KEY_S), &m_down_command);
	Input_handler::instance().add_button_n_cmd(Button(KEY_W), &m_up_command);
	
	glfwSetKeyCallback(window, key_callback);

	int vport_width, vport_height;
	glfwGetFramebufferSize(window, &vport_width, &vport_height);

	glViewport(0, 0, vport_width, vport_height);

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

	cgm::mat4 V, P;
	GLint v_loc, p_loc;
	GLint sampler_loc;
	
	cgm::vec3 t(-floor(tile_map.width() / (float)2), -floor(tile_map.height() / (float)2), 0.0f);
    //V = cgm::rotate_y(180.0f);
	//V.transpose();
	V = cgm::mat4(cgm::mat4(), t);

	P = cgm::ortho(-floor(tile_map.width() / (float)2),  ceil(tile_map.width() / (float)2), -floor(tile_map.height() / (float)2), ceil(tile_map.height() / (float)2), -1.0, 1.0f);

	v_loc = shader.get_uniform_location("V");
	p_loc = shader.get_uniform_location("P");
	sampler_loc = shader.get_uniform_location("tileset1");

	glUniformMatrix4fv(v_loc, 1, GL_FALSE, V.value_ptr());
	glUniformMatrix4fv(p_loc, 1, GL_FALSE, P.value_ptr());
	glUniform1f(sampler_loc, 0);

	///////////////////--------------------------Initialization code------------------ ///////////////////////////////////////////////////////////////////////////
	//Player_idle_state::switch_anim_frames({ 0, 1 });
	//Player_running_state::switch_anim_frames({ 3, 4, 5 });

	std::unique_ptr<Animator_controller> upanim_controller = get_player_anim_controller();


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////// set up sprites////////////////////////////////////////////////////
	gls::Shader sprite_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/sprite.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/sprite.frag");
	sprite_shader.use();
	glUniformMatrix4fv(sprite_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
	glUniformMatrix4fv(sprite_shader.get_uniform_location("P"), 1, GL_FALSE, P.value_ptr());
	glUniform1f(sprite_shader.get_uniform_location("tileset"), 0);

	Player player(cgm::vec3(6.0f, 6.0f), cgm::mat4(), AABB_2d() ,cgm::vec2(1.5f, 1.0f));
	player.set_anim_controller(upanim_controller);

	tgs::Sprite_batch batch(12, player.get_sprite().get_atlas()->get_texture() , sprite_shader);
	batch.add(player.get_sprite());
	
	/////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////// MAP DEBUG SHADERS ////////////////////////////////////////////////////////////////////////
		gls::Shader display_grid_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/d_display_tile_grid.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/d_display_tile_grid.frag");
		display_grid_shader.use();
		glUniformMatrix4fv(display_grid_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
		glUniformMatrix4fv(display_grid_shader.get_uniform_location("P"), 1, GL_FALSE, P.value_ptr());
		map_renderer.set_debug_mode(Tile_map_renderer::Debug_options::DISPLAY_GRID, display_grid_shader);

		gls::Shader display_colliders_shader("C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/d_display_colliders.vert", "C:/Users/mateu/Documents/GitHub/Demos/2d_platformer/source/shaders/d_display_colliders.frag");
		display_colliders_shader.use();
		glUniformMatrix4fv(display_colliders_shader.get_uniform_location("V"), 1, GL_FALSE, V.value_ptr());
		glUniformMatrix4fv(display_colliders_shader.get_uniform_location("P"), 1, GL_FALSE, P.value_ptr());
		
		map_renderer.set_debug_mode(Tile_map_renderer::Debug_options::DISPLAY_COLLIDERS, display_colliders_shader);
	/////////////////////////////////////////////////////////////////////////////////////////

	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	curr_time = last_time = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		
		//last_time = curr_time;
		//curr_time = glfwGetTime();
		Timer::instance().update();
		player.handle_input();
		//float frame_time = curr_time - last_time;
		float frame_time = Timer::instance().get_delta();
		while (frame_time > 0.0f) {
			float delta_time = (frame_time >= dt) ?dt :frame_time;
			player.update(delta_time);
			frame_time -= delta_time;

		}
		
		
		glfwGetFramebufferSize(window, &vport_width, &vport_height);

		glViewport(0, 0, vport_width, vport_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		map_renderer.render(shader);
		
		sprite_shader.use();
		batch.render();
		glBindTexture(GL_TEXTURE_2D, 0);
		glfwPollEvents();
		glfwSwapBuffers(window);

		batch.add(player.get_sprite());
	}
	glfwTerminate();
	return 0;
}