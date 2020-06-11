#ifndef _GAME_OBJECT_MANAGER_HPP
#define _GAME_OBJECT_MANAGER_HPP
#include <map>
#include <vector>
#include <stdint.h>

#include "Game_object.hpp"

/* Game_object_manager: class responsable to instantiate and store all the game objects
   in the world. All the objects are stored in a handle table and should only be accessed by
   handles, that way we can garantee that there will be no danglin pointers. To create new game objects,
   this manager maitains a map that associates the objects type id with a specific creator, each creator
   is able to create a Game_object of the specific type.
 */
namespace math { struct vec3; }
namespace gom {class Creator; class Game_object_handle; class Camera_2d; }
class Event;

namespace gom {

        struct Handle_table_entry {
                uint16_t     m_next_free_index;
                size_t	     m_game_object_sz;
                Game_object  *m_pgame_object;
        };


        class  Game_object_manager final {
                friend Game_object::Game_object(std::size_t);
        public:
                typedef uint32_t	                           	type_id;
                typedef std::map<type_id, Creator*>	           	creator_map;
                typedef std::vector<Game_object*>		        vpgame_objects;
                typedef std::vector<Game_object_handle>         vgame_object_handles;

                Game_object_manager() = default;
                ~Game_object_manager() = default;

                void				        init();
                void				        shut_down();

                bool				        register_creator(const type_id obj_type, Creator *pcreator,
                                                             const uint32_t obj_tag = -1);

                Game_object_handle          instantiate(const type_id obj_type,
                                                        const math::vec3 & wld_pos);

                void				        request_destruction(const Game_object_handle & handle);
                void				        update_game_objects(const float dt);
                void                        reset();

                Game_object			        *get_by_handle(const Game_object_handle & handle);
                vgame_object_handles        find_game_objects_with_type(uint32_t type);

                void                        broadcast_event(Event & event);

                Camera_2d *                 get_main_camera();
        private:
                Game_object_handle          register_game_object(Game_object *pgame_object,
                                                                 std::size_t object_sz);

                void				  destroy_requested_game_objects();

                static const uint16_t      m_MAX_GAME_OBJECTS = 1024;
                static  uint32_t	       m_next_guid;
                Handle_table_entry         m_ahandle_table[m_MAX_GAME_OBJECTS];
                uint16_t				   m_next_free_index;
                creator_map				   m_creators;

                vpgame_objects			   m_game_objects;
                vpgame_objects			   m_game_objects_to_add;
                vgame_object_handles	   m_game_objects_to_destroy;


                Camera_2d               *m_pmain_camera;
        };
        extern Game_object_manager g_game_object_mgr;

        bool  sort_by_guid(const gom::Game_object* lhs, const gom::Game_object* rhs);
}
#endif // !_GAME_OBJECT_MANAGER_HPP
