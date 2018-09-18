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

public:
	typedef std::set<Property> property_set;
	Object();
	Object(const Object & obj);
	Object & operator=(const Object & rhs);
	~Object();

	void set_name(const char * pname);
	void set_type(const char * ptype);

	const char *	get_name() const;
	const char *	get_type() const;
	float		get_x() const;
	float		get_y() const;
	float		get_width() const;
	float		get_height() const;
	float		get_rotation() const;
	const property_set & get_properties() const;

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

inline const char * Object::get_name() const 
{
	return m_name;
}

inline const char * Object::get_type() const 
{
	return m_type;
}

inline float Object::get_x() const
{
	return m_x;
}

inline float Object::get_y() const
{
	return m_y;
}

inline float Object::get_width() const
{
	return m_width;
}

inline float Object::get_height() const
{
	return m_height;
}

inline float Object::get_rotation() const
{
	return m_rotation;
}

inline const Object::property_set & Object::get_properties() const 
{
	return m_properties;
}
#endif // !_OBJECT_HPP
