#ifndef _INPUT_MANAGER_HPP

#define _INPUT_MANAGER_HPP

#include "Button.hpp"
#include <map>
      
#define MAX_NUM_KEYS 40
/*Input_hander: part of the input systems, it maps
 * buttons to especific commands
 *
 */
namespace io {
	enum KEYS { KEY_W = 0, KEY_A, KEY_S, KEY_D, KEY_P, KEY_B, KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP, KEY_ENTER, KEY_SPACE, KEY_Q, KEY_R, INVALID_KEY };
	enum GAME_ACTIONS { JUMP = 0, CLIMB_UP, CLIMB_DOWN, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, ATTACK_01, ATTACK_02, ATTACK_03, ATTACK_04, START, PAUSE, QUIT, RESET };
	extern void key_callback(int key, int scancode, int action, int mods);
	extern void map_action_to_button(const int action, const Button & button);
	extern const Button & get_button_from_action(const int action) ;
}


//class Input_manager {
//public:
	//Input_manager() = default;
//	~Input_manager() {}
	//void init();
	//void shut_down();
	//enum KEYS {KEY_W = 0, KEY_A, KEY_S, KEY_D, KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP, KEY_ENTER, KEY_SPACE, KEY_Q, INVALID_KEY};
	//enum GAME_ACTIONS { JUMP = 0, CLIMB_UP, CLIMB_DOWN, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, ATTACK_01, ATTACK_02, ATTACK_03, ATTACK_04, START, PAUSE, QUIT };
	//void key_callback(int key, int scancode, int action, int mods);
	//void map_action_to_button(const int action, const Button & button);
	//const Button & get_button_from_action(const int action) const;
//private:
//	Button   m_buttons[MAX_NUM_KEYS];
//	std::map<int, int> m_action_to_key;
//};

#endif // !_INPUT_MANAGER_HPP
