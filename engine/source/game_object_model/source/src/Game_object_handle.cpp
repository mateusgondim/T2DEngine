#include "Game_object_handle.hpp"
#include "Game_object.hpp"

namespace gom {
	Game_object_handle::Game_object_handle(const Game_object & game_object) 
	{
		m_unique_id		=  game_object.get_unique_id();
		m_handle_index  =  game_object.get_handle_index();
	}
}