#ifndef _GAME_OBJECT_MANAGER_HPP
#define _GAME_OBJECT_MANAGER_HPP
#include <map>
#include <stdint.h>


/* Game_object_manager: class responsable to instantiate and store all the game objects
   in the world. All the objects are stored in a handle table and should only be accessed by
   handles, that way we can garantee that there will be no danglin pointers. To create new game objects,
   this manager maitains a map that associates the objects type id with a specific creator, each creator 
   is able to create a Game_object of the specific type.
 */

namespace gom { class Game_object; class Creator; class Game_object_handle; }

namespace gom {
	
	struct Handle_table_entry {
		uint16_t     m_next_free_index;
		size_t	     m_game_object_sz;
		Game_object  *m_pgame_object;
	};


	class  Game_object_manager final {
	public:
		typedef uint16_t						type_id;
		typedef std::map<type_id, Creator*>		creator_map;

		//constructor and destructor
		Game_object_manager() = default;
		~Game_object_manager() = default;

		void				  init();
		void				  shut_down();

		bool				  register_creator(const type_id obj_type, Creator *pcreator);
		Game_object_handle    instantiate(const type_id obj_type);
		void				  destroy(const Game_object_handle & handle);

		Game_object			  *get_by_handle(const Game_object_handle & handle);

	private:
		static const uint16_t  m_MAX_GAME_OBJECTS = 1024;
		static  uint32_t	   m_next_guid;
		Handle_table_entry     m_ahandle_table[m_MAX_GAME_OBJECTS];
		uint16_t               m_next_free_index;
		creator_map			   m_creators;
	};
	extern Game_object_manager g_game_object_mgr;
}
#endif // !_GAME_OBJECT_MANAGER_HPP
