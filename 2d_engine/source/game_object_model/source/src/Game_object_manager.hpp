#ifndef _GAME_OBJECT_MANAGER_HPP
#define _GAME_OBJECT_MANAGER_HPP

#include "string_id.hpp"
#include "Pool_allocator.hpp"
#include <map>

#define MAX_NUM_POOLS 6

/* Game_object_manager: class responsable for managing and spawning all the game objects in the world,
 * it can register new creator objects for spawning new types of game objects using the register function.
 * All the game objects are stored using the small pool allocators stored in the array. To keep track of all the objects in the world
 * the manager keeps a map of addresses of each objects and the corresponding pool allocator it memory belongs to.
 */

class Game_object;
class Creator;

class  Game_object_manager final {
	typedef string_id		type_id;
	typedef std::size_t		pool_array_id;

	//constructor and destructor
	Game_object_manager() = default;
	~Game_object_manager() = default;

	void init();
	void shut_down();
	
	bool		register_creator(const type_id obj_type, Creator *pcreator);
	Game_object *create_game_object(const type_id obj_type);
	void        release_game_object(Game_object *pobj);
	//print_pools_stats

private:
	std::map<type_id, Creator*>		m_creator_map;
	std::map<void*, pool_array_id>	m_objects_map;
	Pool_allocator					m_pool_array[MAX_NUM_POOLS];
};

#endif // !_GAME_OBJECT_MANAGER_HPP
