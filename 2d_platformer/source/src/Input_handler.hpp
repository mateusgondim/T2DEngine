#ifndef _INPUT_HANDLER_HPP

#define _INPUT_HANDLER_HPP
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Button.hpp"
#include "Command.hpp"
#include <vector>
#include <utility>

#define KEY_W		0
#define KEY_A		1
#define	KEY_S		2
#define KEY_D		3
#define KEY_ENTER	4

/*Input_hander: part of the input systems, it maps
 * buttons to especific commands
 *
 */
class Input_handler {
public:
public:
	Input_handler(const Input_handler &) = delete;
	static Input_handler & instance()
	{
		static Input_handler * instance = new Input_handler();
		return *instance;
	}
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void add_button_n_cmd(const Button &,  Command *);
	const std::vector<std::pair<Button, Command*>> & get_input() const { return m_buttons_to_cmd; }
private:
	Input_handler() = default;
	std::vector<std::pair<Button, Command*>> m_buttons_to_cmd;
};


#endif // !_INPUT_HANDLER_HPP
