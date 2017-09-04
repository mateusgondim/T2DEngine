#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>
#include "Input_handler.hpp"

void Input_handler::add_button_n_cmd(const Button & b,  Command * cmd) 
{
	auto iter = std::find_if(m_buttons_to_cmd.begin(), m_buttons_to_cmd.end(),
		[b](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == b.m_bound_key; });

	if (iter == m_buttons_to_cmd.end()) {
		const std::pair<Button, Command *> p(b, cmd);
		m_buttons_to_cmd.push_back(p);
#ifndef NDEBUG
		std::cout << "added button bound to key: ";
		switch (b.m_bound_key) {
		case KEY_W:
			std::cout << "w" << std::endl;
			break;
		case KEY_A:
			std::cout << "a" << std::endl;
			break;
		case KEY_S:
			std::cout << "s" << std::endl;
			break;
		case KEY_D:
			std::cout << "d" << std::endl;
			break;
		case KEY_ENTER:
			std::cout << "enter" << std::endl;
			break;
		default:
			std::cerr << "unkown keycode" << std::endl;
			return;
		}
#endif // !NDEBUG

	}
}

void Input_handler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Button button;
#ifndef NDEBUG
	std::string key_name, action_name;
#endif // !NDEBUG

	//find out witch key was pressed
	switch (key) {
	case GLFW_KEY_W:
#ifndef NDEBUG
		key_name = "w";
#endif // !NDEBUG
		button.m_bound_key = KEY_W;
		break;
	case GLFW_KEY_A:
#ifndef NDEBUG
		key_name = "a";
#endif // !NDEBUG
		button.m_bound_key = KEY_A;
		break;
	case GLFW_KEY_S:
#ifndef NDEBUG
		key_name = "s";
#endif // !NDEBUG
		button.m_bound_key = KEY_S;
		break;
	case GLFW_KEY_D:
#ifndef NDEBUG
		key_name = "d";
#endif // !NDEBUG
		button.m_bound_key = KEY_D;
		break;
	case GLFW_KEY_ENTER:
#ifndef NDEBUG
		key_name = "enter";
#endif // !NDEBUG
		button.m_bound_key = KEY_ENTER;
		break;
	default:
		std::cerr << __FUNCTION__ << " : unkown keycode" << std::endl;
		return;
	}
	
	//get action
	int new_action;
	switch (action) {
	case GLFW_RELEASE:
#ifndef NDEBUG
		action_name = "Released";
#endif // !NDEBUG
		new_action = RELEASED;
		break;
	case GLFW_PRESS:
#ifndef NDEBUG
		action_name = "Pressed";
#endif // !NDEBUG
		new_action = PRESSED;
		break;
	case GLFW_REPEAT:
#ifndef NDEBUG
		action_name = "Repeat";
#endif // !NDEBUG
		new_action = REPEAT;
		break;
	}

#ifndef NDEBUG
	std::cout << "DEBUG: " << "key: " << key_name << " | action: " << action_name << std::endl;
#endif // !NDEBUG


	//find if the key is already on the buffer
	auto iter = std::find_if(m_buttons_to_cmd.begin(), m_buttons_to_cmd.end(), 
		[button](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == button.m_bound_key; });
	//auto iter = m_buttons_to_cmd.find(button);
	if (iter != m_buttons_to_cmd.end()) {
		(iter ->first).update(new_action);
	}
	
}