#include "Game_object_manager.hpp"

#include "Game_object.hpp"
#include "Creator.hpp"
#include "Game_object_handle.hpp"
#include "runtime_memory_allocator.hpp"
#include "Camera_2d.hpp"

#include "vec3.hpp"

#include "Sprite_atlas_manager.hpp"
#include "Physics_manager.hpp"
#include "Body_2d.hpp"

#include <map>
#include <algorithm>
#include <cassert>
#include <iostream>

namespace gom
{
        Game_object_manager g_game_object_mgr;

        uint32_t	   Game_object_manager::m_next_guid = 1; // zero is reserved for invalid guid

        void Game_object_manager::init()
        {
                //set up the handle table
                m_next_free_index = 0;
                for (size_t i = 0; i < m_MAX_GAME_OBJECTS - 1; ++i) {
                        m_ahandle_table[i].m_next_free_index = i + 1;
                        m_ahandle_table[i].m_game_object_sz = 0;
                        m_ahandle_table[i].m_pgame_object = nullptr;
                }
                // the last index is considere invalid, i.e, when the m_next_free_index= m_MAX_GAME_OBJECTS - 1, the table is considered full
                m_ahandle_table[m_MAX_GAME_OBJECTS - 1].m_next_free_index = 0;
                m_ahandle_table[m_MAX_GAME_OBJECTS - 1].m_game_object_sz = 0;
                m_ahandle_table[m_MAX_GAME_OBJECTS - 1].m_pgame_object = nullptr;

                //set the capacity of the vectors to prevend allocations at runtime
                m_game_objects.reserve(m_MAX_GAME_OBJECTS / 3);
                m_game_objects_to_add.reserve(m_MAX_GAME_OBJECTS / 5);
                m_game_objects_to_destroy.reserve(m_MAX_GAME_OBJECTS / 5);
        }

        void Game_object_manager::shut_down()
        {
                //destroy and deallocate all the game objects 
                for (size_t i = 0; i < m_MAX_GAME_OBJECTS; ++i) {
                        if (m_ahandle_table[i].m_pgame_object != nullptr) {
                                m_ahandle_table[i].m_pgame_object-> ~Game_object();
                                mem::free(static_cast<void*>(m_ahandle_table[i].m_pgame_object), m_ahandle_table[i].m_game_object_sz);
                                m_ahandle_table[i].m_pgame_object = nullptr;
                        }
                }

                //delete creators
                for (creator_map::iterator it = m_creators.begin(); it != m_creators.end(); ++it) {
                        delete (it->second);
                }

                // deallocate main camera
                if (m_pmain_camera) {
                        delete m_pmain_camera;
                }
        }

        bool	Game_object_manager::register_creator(const type_id obj_type, Creator *pcreator, const uint32_t obj_tag)
        {
                //check if there is already a creator assigned to this id
                creator_map::iterator it = m_creators.find(obj_type);
                if (it != m_creators.end()) { // creator alredy assigned to this type
                        delete pcreator;
                        return false;
                }
                //set the creator's object's type
                pcreator->set_obj_type(obj_type);

                if (obj_tag != -1) {
                        pcreator->set_obj_tag(obj_tag);
                }
                // add entry on the map
                m_creators[obj_type] = pcreator;

                return true;
        }

        Game_object_handle Game_object_manager::instantiate(const type_id obj_type, const math::vec3 & wld_pos)
        {
                //find the creator associated with this type's id
                creator_map::iterator it = m_creators.find(obj_type);
                if (it == m_creators.end()) {
                        std::cerr << "ERROR(" << __FUNCTION__ << "): invalid type_id = " << obj_type << std::endl;
                        return Game_object_handle();
                }

                //check if there is space in the table 
                if (m_next_free_index == m_MAX_GAME_OBJECTS - 1) {
                        std::cerr << "ERROR(" << __FUNCTION__ << "): Handle table is full" << std::endl;
                        return Game_object_handle();
                }

                //get the next free handle index for storing this game object
                uint16_t handle_index = m_next_free_index;

                //generate a new unique id for this object
                uint32_t unique_id = m_next_guid;

                // get the object's size in bytes
                std::size_t obj_sz = (it->second)->get_size();

                //get a memory block large enough to store the object
                void *pmem = mem::allocate(obj_sz);

                if (pmem == nullptr) {
                        std::cerr << "ERROR(" << __FUNCTION__ << "): Could not allocate space for a new Game Object" << std::endl;
                        return Game_object_handle();
                }

                // create the requested Game Object
                Game_object * pgame_obj = it->second->create(pmem, m_next_guid, handle_index, wld_pos);

                if (pgame_obj != nullptr) {
                        //if the creation was succefull, update the next_guid and next_handle
                        m_next_free_index = m_ahandle_table[m_next_free_index].m_next_free_index;
                        ++m_next_guid;

                        //set the handle entry data
                        m_ahandle_table[handle_index].m_next_free_index = 0;//invalid 
                        m_ahandle_table[handle_index].m_game_object_sz = obj_sz;
                        m_ahandle_table[handle_index].m_pgame_object = pgame_obj;

                        //add it to the vector of game objects to be added to the main vector
                        m_game_objects_to_add.push_back(pgame_obj);

                        //set to be inactive, is not yet on the game
                        pgame_obj->set_active(false);

                        //return the handle for the created game object
                        return Game_object_handle(*pgame_obj);
                }
                else {
                        //give back the memory
                        mem::free(pmem, obj_sz);
                        std::cerr << "ERROR(" << __FUNCTION__ << "): Could not create game object" << std::endl;
                        return Game_object_handle();
                }

        }

        void Game_object_manager::request_destruction(const Game_object_handle & handle)
        {
                //check the handle entry from the table
                Game_object *pgame_object = get_by_handle(handle);

                if (pgame_object != nullptr) {
                        // the Game object is on the table, add the handle to the vector of objects to be destroyed
                        m_game_objects_to_destroy.push_back(handle);
                }
        }

        void Game_object_manager::destroy_requested_game_objects()
        {
                if (!m_game_objects_to_destroy.empty()) {
                        //destroyed all the requested game_objects
                        vgame_object_handles::iterator beg = m_game_objects_to_destroy.begin();
                        vgame_object_handles::iterator end = m_game_objects_to_destroy.end();
                        Game_object *pgame_object = nullptr;
                        for (; beg != end; ++beg) {
                                //get the game object by handle. OBS: dont need to check, all the handles on the vector a valid
                                pgame_object = m_ahandle_table[(*beg).m_handle_index].m_pgame_object;

                                // remove the game object from the vector of all the game objects in the world
                                vpgame_objects::iterator it = std::lower_bound(m_game_objects.begin(), m_game_objects.end(), pgame_object, sort_by_guid);
                                if (it != m_game_objects.end() && ((*it)->get_unique_id() == pgame_object->get_unique_id())) {
                                        m_game_objects.erase(it);
                                }

                                // call the destructor 
                                pgame_object->~Game_object();

                                //free the Game_object's memory block
                                mem::free(static_cast<void*>(pgame_object), m_ahandle_table[(*beg).m_handle_index].m_game_object_sz);
                                m_ahandle_table[(*beg).m_handle_index].m_pgame_object = nullptr;

                                //update the table
                                m_ahandle_table[(*beg).m_handle_index].m_next_free_index = m_next_free_index;
                                m_next_free_index = (*beg).m_handle_index;
                        }

                        //clear the vector
                        m_game_objects_to_destroy.clear();
                }
                /*
                //check the handle entry from the table
                Game_object *pgame_object = get_by_handle(handle);

                if (pgame_object != nullptr) {
                    //the Game object is on the table

                    // call the destructor
                    pgame_object->~Game_object();

                    //free the Game_object's memory block
                    mem::free(static_cast<void*>(pgame_object), m_ahandle_table[handle.m_handle_index].m_game_object_sz);
                    m_ahandle_table[handle.m_handle_index].m_pgame_object = nullptr;

                    //update the table
                    m_ahandle_table[handle.m_handle_index].m_next_free_index = m_next_free_index;
                    m_next_free_index = handle.m_handle_index;
                }
                */
        }
        //destroy all the game objects in the world	
        void Game_object_manager::reset()
        {
                m_game_objects_to_destroy.clear();
                //check if there are objects that were added in the previous frame
                if (!m_game_objects_to_add.empty()) {
                        vpgame_objects::iterator it = m_game_objects_to_add.begin();
                        //destroy game objects
                        for (; it != m_game_objects_to_add.end(); ++it) {
                                //get the objects'handle
                                uint16_t handle_index = (*it)->get_handle_index();
                                //destroy game object

                              // call the destructor 
                                (*it)->~Game_object();

                                //free the Game_object's memory block
                                mem::free(static_cast<void*>(*it), m_ahandle_table[handle_index].m_game_object_sz);
                                m_ahandle_table[handle_index].m_pgame_object = nullptr;

                                //update the table
                                m_ahandle_table[handle_index].m_next_free_index = m_next_free_index;
                                m_next_free_index = handle_index;

                        }
                        //clear vector
                        m_game_objects_to_add.clear();
                }
                if (!m_game_objects.empty()) {
                        vpgame_objects::iterator it = m_game_objects.begin();
                        //destroy game objects
                        for (; it != m_game_objects.end(); ++it) {
                                //get the objects'handle
                                uint16_t handle_index = (*it)->get_handle_index();
                                //destroy game object

                              // call the destructor 
                                (*it)->~Game_object();

                                //free the Game_object's memory block
                                mem::free(static_cast<void*>(*it), m_ahandle_table[handle_index].m_game_object_sz);
                                m_ahandle_table[handle_index].m_pgame_object = nullptr;

                                //update the table
                                m_ahandle_table[handle_index].m_next_free_index = m_next_free_index;
                                m_next_free_index = handle_index;
                        }
                        //clear vector
                        m_game_objects.clear();
                }
                //reset guid counter
                m_next_guid = 1;
        }

        Game_object *Game_object_manager::get_by_handle(const Game_object_handle & handle)
        {
                assert(handle.m_handle_index < m_MAX_GAME_OBJECTS - 1);

                //check the handle entry from the table
                Game_object *pgame_object = m_ahandle_table[handle.m_handle_index].m_pgame_object;
                if (pgame_object != nullptr) {
                        return (pgame_object->get_unique_id() == handle.m_unique_id) ? (pgame_object) : (nullptr);
                }
                else {
                        return nullptr;
                }
        }

        Game_object_manager::vgame_object_handles Game_object_manager::find_game_objects_with_type(uint32_t type)
        {
                vgame_object_handles            handles;
                vpgame_objects::iterator        it = m_game_objects.begin();
                for (; it != m_game_objects.end(); ++it) {
                        if (type == (*it)->get_type()) {
                                handles.push_back(Game_object_handle(*(*it)));
                        }
                }
                return handles;
        }

        void Game_object_manager::update_game_objects(const float dt)
        {
                vpgame_objects::iterator b;
                if (!m_game_objects_to_add.empty()) {
                        // update the world's game object vector by adding the objects that were created on previous frame
                        b = m_game_objects_to_add.begin();
                        for (; b != m_game_objects_to_add.end(); ++b) {
                                (*b)->set_active(true);
                                m_game_objects.insert(std::upper_bound(m_game_objects.begin(), m_game_objects.end(), *b, sort_by_guid), *b);
                        }
                        m_game_objects_to_add.clear();
                }
                //process the the destruction requests
                destroy_requested_game_objects();

                b = m_game_objects.begin();
                for (; b != m_game_objects.end(); ++b) {
                        if ((*b)->is_active()) {
                                (*b)->update(dt);
                        }
                }
        }

        bool gom::sort_by_guid(const gom::Game_object* lhs, const gom::Game_object* rhs)
        {
                return lhs->get_unique_id() < rhs->get_unique_id();
        }

        void Game_object_manager::set_main_camera(gom::Camera_2d * pmain_camera)
        {
                m_pmain_camera = pmain_camera;
        }

        gom::Camera_2d * Game_object_manager::get_main_camera()
        {
                return m_pmain_camera;
        }
}