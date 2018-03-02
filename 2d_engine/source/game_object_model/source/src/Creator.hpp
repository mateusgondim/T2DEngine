#ifndef _CREATOR_HPP
#define _CREATOR_HPP

#include <cstddef>

/* Creator: this class is used with the Game_object_factory class, to be able to dynamically create objects at runtime 
 * Every class that wants to register itself with the factory, should have a correspoding Creator class, i.e
 * the Zombie class should have a Creator_zombie that inherits from the Creator
 */

class Game_object;

class Creator {
public:
	Creator(const std::size_t sz) : m_size(sz) {}
	virtual ~Creator() = default;

	virtual Game_object *create(void * pmem) = 0;
	std::size_t get_size() const { return m_size; }
private:
	std::size_t m_size; // size in bytes
};

#endif // !_CREATOR_HPP
