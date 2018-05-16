#ifndef _GAME_OBJECT_HANDLE_HPP
#define _GAME_OBJECT_HANDLER_HPP
#include <stdint.h>

namespace gom { class  Game_object;  class Game_object_manager; }

namespace gom {
	class Game_object_handle {
	friend Game_object_manager;
	public:
		typedef uint32_t game_object_id;
		Game_object_handle() : m_unique_id(0), m_handle_index(0) {} // 0 is a invalid guid
		explicit Game_object_handle(const Game_object & game_object);
	private:
		game_object_id m_unique_id;
		uint16_t	   m_handle_index;
	};
}

#endif // !_GAME_OBJECT_HANDLE_HPP
