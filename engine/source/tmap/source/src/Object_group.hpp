#ifndef _OBJECT_GROUP_HPP
#define _OBJECT_GROUP_HPP

#include <vector>
#include <iostream>
#include "Object.hpp"

class Object_group {
friend  Tile_map;
public:
	Object_group();
	Object_group(const Object_group &) = delete;
	Object_group & operator=(const Object_group &) = delete;
	~Object_group();
	const std::vector<Object*> & get_objects() const;
	void		set_name(const char *str);
	const char *	get_name() const;
private:
	std::vector<Object*>	m_objects;
	char *		m_name;
};

std::ostream & operator<< (std::ostream & os, const Object_group & obj_group);

inline const std::vector<Object*> & Object_group::get_objects() const 
{
	return m_objects;
}

inline const char * Object_group::get_name() const
{
	return m_name;
}



#endif // !_OBJECT_GROUP_HPP
