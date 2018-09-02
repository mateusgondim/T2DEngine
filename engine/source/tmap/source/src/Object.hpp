#ifndef _OBJECT_HPP
#define _OBJECT_HPP
#include <stdint.h>
#include <set>
#include <iostream>
#include "Property.hpp"

class Tile_map;

class Object {
friend class Tile_map;
friend std::ostream & operator<<(std::ostream & os, const Object & object);

typedef std::set<Property> property_set;

public:
	Object();
	Object(const Object & obj);
	Object & operator=(const Object & rhs);
	~Object();

	void set_name(const char * pname);
	void set_type(const char * ptype);
private:
	uint32_t	m_id;
	float		m_x;
	float		m_y;
	float		m_width;
	float		m_height;
	uint32_t	m_gid;
	float		m_rotation;
	property_set	m_properties;
	char *		m_name;
	char *		m_type;
};

std::ostream & operator<<(std::ostream & os, const Object & object);


#endif // !_OBJECT_HPP
