#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <algorithm>
#include <map>
#include <utility>
#include <cassert>
#include <string>
#include "Input_manager.hpp"


void Input_manager::map_action_to_button(const int action, const Button & button) 
{
	assert(button.m_bound_key >= 0 && " key value is negative");
	assert(button.m_bound_key < MAX_NUM_KEYS && " key value is greater than the maximun number of keys");
	
	m_action_to_key[action] = button.m_bound_key; // map the action to the button
	m_buttons[button.m_bound_key] = button;
	
#ifndef NDEBUG
	std::cout << __FUNCTION__ << " : action ";
	switch (action) {
	case MOVE_LEFT:
		std::cout << "MOVE_LEFT ";
		break;
	case MOVE_RIGHT:
		std::cout << "MOVE_RIGHT ";
		break;
	case MOVE_UP:
		std::cout << "MOVE_UP ";
		break;
	case MOVE_DOWN:
		std::cout << "MOVE_DOWN ";
		break;
	case JUMP:
		std::cout << "JUMP ";
		break;
	case ATTACK_01:
		std::cout << "ATTACK_01 ";
		break;
	case ATTACK_02:
		std::cout << "ATTACK_02 ";
		break;
	case ATTACK_03:
		std::cout << "ATTACK_03 ";
		break;
	case ATTACK_04:
		std::cout << "ATTACK_04 ";
		break;
	case START:
		std::cout << "START ";
		break;
	case PAUSE:
		std::cout << "PAUSE ";
		break;
	case QUIT:
		std::cout << "QUIT ";
		break;
	default:
		std::cout << "UNKOWN ACTION ";
	}
	std::cout << "To Key: ";
	switch (button.m_bound_key) {
	case KEY_W:
		std::cout << "W" << std::endl;
		break;
	case KEY_A:
		std::cout << "A" << std::endl;
		break;
	case KEY_S:
		std::cout << "S" << std::endl;
		break;
	case KEY_D:
		std::cout << "D" << std::endl;
		break;
	case KEY_ENTER:
		std::cout << "ENTER" << std::endl;
		break;
	case KEY_SPACE:
		std::cout << "SPACE" << std::endl;
		break;
	case KEY_Q:
		std::cout << "Q" << std::endl;
	default:
		std::cerr << "UNKWON KEYCODE" << std::endl;
		return;
	}
#endif // !NDEBUG
}

const Button & Input_manager::get_button_from_action(const int action) const
{
	std::map<int, int>::const_iterator iter = m_action_to_key.find(action);
	if (iter != m_action_to_key.cend()) {
		return m_buttons[iter->second];
	}
#ifndef NDEBUG
	std::cout << __FUNCTION__ << ": There is no key mapped to the requested action" << std::endl;
	return Button(INVALID_KEY);
#endif // !NDEBUG
}
/*
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
}*/

void Input_manager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
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
	case GLFW_KEY_SPACE:
#ifndef NDEBUG
		key_name = "SPACE";
#endif // !NDEBUG
		button.m_bound_key = KEY_SPACE;
		break;
	case GLFW_KEY_Q:
#ifndef NDEBUG
		key_name = "Q";
#endif // !NDEBUG
		button.m_bound_key = KEY_Q;
		break;
	case GLFW_KEY_LEFT:
#ifndef NDEBUG
		key_name = "LEFT";
#endif // !NDEBUG
		button.m_bound_key = KEY_LEFT;
		break;
	case GLFW_KEY_RIGHT:
#ifndef NDEBUG
		key_name = "RIGHT";
#endif // !NDEBUG
		button.m_bound_key = KEY_RIGHT;
		break;
	case GLFW_KEY_DOWN:
#ifndef NDEBUG
		key_name = "DOWN";
#endif // !NDEBUG
		button.m_bound_key = KEY_DOWN;
		break;
	case GLFW_KEY_UP:
#ifndef NDEBUG
		key_name = "UP";
#endif // !NDEBUG
		button.m_bound_key = KEY_UP;
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
	//std::cout << __FUNCTION__ << " DEBUG: " << "key: " << key_name << " | action: " << action_name << std::endl;
#endif // !NDEBUG

	m_buttons[button.m_bound_key].update(new_action);
	/*
	//find if the key is already on the buffer
	auto iter = std::find_if(m_buttons_to_cmd.begin(), m_buttons_to_cmd.end(), 
		[button](const std::pair<Button, Command*> & p) {return p.first.m_bound_key == button.m_bound_key; });
	//auto iter = m_buttons_to_cmd.find(button);
	if (iter != m_buttons_to_cmd.end()) {
		(iter ->first).update(new_action);
	}*/
	
}