#include "Game_object_manager.hpp"

#include "Game_object.hpp"
#include "Creator.hpp"
#include "string_id.hpp"
#include "Pool_allocator.hpp"

#include <map>
#include <cmath>
#include <iostream>

//Initilaze all the memory pools 
void	Game_object_manager::init() 
{
	std::size_t min_pool_element_sz = 8;
	std::size_t alignment = 4;
	std::size_t num_elements = 20;

	for (std::uint32_t i = 0; i < MAX_NUM_POOLS; ++i) {
		std::size_t pool_element_sz = min_pool_element_sz * std::pow(2, i);
		m_pool_array[i].alloc_pool(pool_element_sz, num_elements, alignment);
	}
}

//REMEMBER THE OBJECTS MUST BE DESTROYED BEFORE THE MEMORY IS RELEASED
void	Game_object_manager::shut_down() 
{
	for (std::size_t i = 0; i < MAX_NUM_POOLS; ++i) {
//		m_pool_array[i].realease_pool_mem();
		;
	}
}

bool	Game_object_manager::register_creator(const type_id obj_type, Creator *pcreator) 
{
	//check if there is already a creator assigned to this id
	std::map<type_id, Creator*>::iterator it = m_creator_map.find(obj_type);
	if (it != m_creator_map.end()) { // creator alredy assigned to this type
		delete pcreator;
		return false;
	}
	// add entry on the map
	m_creator_map[obj_type] = pcreator;

	return true;
}

Game_object	*Game_object_manager::create_game_object(const type_id obj_type) 
{
	//find the creator associated with this type's id
	std::map<type_id, Creator*>::iterator it = m_creator_map.find(obj_type);
	if (it == m_creator_map.end()) {
		std::cerr << "ERROR(" << __FUNCTION__ << "): invalid type_id = " << obj_type << std::endl;
		return nullptr;
	}

	// get the object's size in bytes
	std::size_t obj_sz = (it->second)->get_size();

	//find the pool allocator with the smallest element size that can store this object
	pool_array_id pool_index = -1;
	for (std::size_t i = 0; i < MAX_NUM_POOLS; ++i) {
		if (obj_sz <= m_pool_array[i].get_element_size()) {
			pool_index = i;
			break;
		}
	}

	if (pool_index == -1) { // object size is larger than the element size of all the pools in the array
		std::cerr << "ERROR(" << __FUNCTION__ << "): there is no pool with element size greater of equal to " << obj_sz << " bytes" << std::endl;
		return nullptr;
	}

	//request the memory block from the pool allocator
	void *pmem = m_pool_array[pool_index].get_element();
	if (pmem == nullptr) { // pool is full
		return nullptr;
	}

	// store the pointer to the game object's memory location and, the corresponding pool id
	m_objects_map[pmem] = pool_index;

	// create the requested Game Object
	Game_object * game_obj = it->second->create(pmem);

	return game_obj;
}

void Game_object_manager::release_game_object(Game_object *pobj) 
{
	//get void* to the object's memory block
	void *pmem = static_cast<void*>(pobj);

	// see if this object was allocated by the manager
	std::map<void*, pool_array_id>::iterator it = m_objects_map.find(pmem);

	if (it == m_objects_map.end()) {
		std::cerr << "ERROR(" << __FUNCTION__ << "): This game object was not allocated by this class or this is a dangling pointer" << std::endl;
		return;
	}
	//get the index of the pool that owns the object's memory
	pool_array_id index = it->second;
	
	//release the pool's element
	m_pool_array[index].free_element(pmem);

	//remove the object's memory address and pool id from the map
	m_objects_map.erase(pmem);
}