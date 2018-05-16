#include "Game_object_manager.hpp"

#include "Game_object.hpp"
#include "Creator.hpp"
#include "Game_object_handle.hpp"
#include "runtime_memory_allocator.hpp"

#include "Sprite_atlas_manager.hpp"
#include "World.hpp"
#include "Body_2d.hpp"

#include <map>
#include <cassert>
#include <iostream>

namespace gom {
	uint32_t	   Game_object_manager::m_next_guid		= 1; // zero is reserved for invalid guid
	
	void Game_object_manager::init(gfx::Sprite_atlas_manager *patlas_manager, physics_2d::World *pwld)
	{
		//set managers
		m_patlas_manager = patlas_manager;
		m_pworld		 = pwld;
		
		//set up the handle table
		m_next_free_index = 0;
		for (size_t i = 0; i < m_MAX_GAME_OBJECTS - 1; ++i) {
			m_ahandle_table[i].m_next_free_index =  i + 1;
			m_ahandle_table[i].m_game_object_sz  =  0;
			m_ahandle_table[i].m_pgame_object    =  nullptr;
		}
		// the last index is considere invalid, i.e, when the m_next_free_index= m_MAX_GAME_OBJECTS - 1, the table is considered full
		m_ahandle_table[m_MAX_GAME_OBJECTS - 1].m_next_free_index = 0;
		m_ahandle_table[m_MAX_GAME_OBJECTS - 1].m_game_object_sz = 0;
		m_ahandle_table[m_MAX_GAME_OBJECTS - 1].m_pgame_object = nullptr;
	}

	void Game_object_manager::shut_down() 
	{
		//destroy and deallocate all the game objects 
		for (size_t i = 0; i < m_MAX_GAME_OBJECTS; ++i) {
			if (m_ahandle_table[i].m_pgame_object != nullptr) {
				physics_2d::Body_2d *pbody = m_ahandle_table[i].m_pgame_object->get_body_2d_component();
				m_pworld->destroy_body_2d(pbody);

				m_ahandle_table[i].m_pgame_object-> ~Game_object();
				mem::free(static_cast<void*>(m_ahandle_table[i].m_pgame_object), m_ahandle_table[i].m_game_object_sz);
				m_ahandle_table[i].m_pgame_object = nullptr;
			}
		}

		//delete creators
		for (creator_map::iterator it = m_creators.begin(); it != m_creators.end(); ++it) {
			delete (it->second);
		}
	}

	bool	Game_object_manager::register_creator(const type_id obj_type, Creator *pcreator)
	{
		//check if there is already a creator assigned to this id
		creator_map::iterator it = m_creators.find(obj_type);
		if (it != m_creators.end()) { // creator alredy assigned to this type
			delete pcreator;
			return false;
		}
		// add entry on the map
		m_creators[obj_type] = pcreator;

		return true;
	}
	
	Game_object_handle Game_object_manager::instantiate(const type_id obj_type) 
	{
		//find the creator associated with this type's id
		creator_map::iterator it = m_creators.find(obj_type);
		if (it == m_creators.end()) {
			std::cerr << "ERROR(" << __FUNCTION__ << "): invalid type_id = " << obj_type << std::endl;
			return Game_object_handle();
		}
		
		//check if there is space in the table 
		if (m_next_free_index == m_MAX_GAME_OBJECTS - 1) {
			std::cerr << "ERROR(" << __FUNCTION__ << "): Handle table is full"<< std::endl;
			return Game_object_handle();
		}

		//get the next free handle index for storing this game object
		uint16_t handle_index = m_next_free_index;
		
		//generate a new unique id for this object
		uint32_t unique_id    = m_next_guid;
		
		// get the object's size in bytes
		std::size_t obj_sz = (it->second)->get_size();

		//get a memory block large enough to store the object
		void *pmem = mem::allocate(obj_sz);

		if (pmem == nullptr) {
			std::cerr << "ERROR(" << __FUNCTION__ << "): Could not allocate space for a new Game Object" << std::endl;
			return Game_object_handle();
		}

		// create the requested Game Object
		Game_object * pgame_obj = it->second->create(pmem, m_next_guid, handle_index, m_patlas_manager, m_pworld);

		if (pgame_obj != nullptr) {
			//if the creation was succefull, update the next_guid and next_handle
			m_next_free_index = m_ahandle_table[m_next_free_index].m_next_free_index;
			++m_next_guid;

			//set the handle entry data
			m_ahandle_table[handle_index].m_next_free_index =  0;//invalid 
			m_ahandle_table[handle_index].m_game_object_sz  =  obj_sz;
			m_ahandle_table[handle_index].m_pgame_object    =  pgame_obj;

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

	void Game_object_manager::destroy(const Game_object_handle & handle) 
	{
		//check the handle entry from the table
		Game_object *pgame_object = get_by_handle(handle);

		if (pgame_object != nullptr) {
			
			//the Game object is on the table, first deallocate the object's body_2d component. We have to do it here, because we need the pointer to the world object
			physics_2d::Body_2d *pbody = pgame_object->get_body_2d_component();
			m_pworld->destroy_body_2d(pbody);
			
			// call the destructor 
			pgame_object->~Game_object();
			
			//free the Game_object's memory block
			mem::free(static_cast<void*>(pgame_object), m_ahandle_table[handle.m_handle_index].m_game_object_sz);
			m_ahandle_table[handle.m_handle_index].m_pgame_object = nullptr;
				
			//update the table
			m_ahandle_table[handle.m_handle_index].m_next_free_index = m_next_free_index;
			m_next_free_index = handle.m_handle_index;
		}
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
	
}