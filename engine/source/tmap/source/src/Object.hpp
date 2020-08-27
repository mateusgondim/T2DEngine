#ifndef _OBJECT_HPP
#define _OBJECT_HPP
#include <stdint.h>
#include <set>
#include <iostream>

#include "Property.hpp"
#include "string_id.hpp"

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
    void set_name_id(const string_id name_id);
	void set_type(const char * ptype);
    void set_type_id(const string_id type_id);

	const char *	get_name() const;
	const char *	get_type() const;
	float		get_x() const;
	float		get_y() const;
	float		get_width() const;
	float		get_height() const;
	float		get_rotation() const;
	const property_set & get_properties() const;

    string_id get_name_id() const;
    string_id get_type_id() const;
private:
	uint32_t	m_id;
	float		m_x;
	float		m_y;
	float		m_width;
	float		m_height;
	uint32_t	m_gid;
	float		m_rotation;
	property_set	m_properties;
    string_id   m_name_id;
    string_id   m_type_id;
	char *		m_name;
	char *		m_type;
};

std::ostream & operator<<(std::ostream & os, const Object & object);

inline const char * Object::get_name() const 
{
	return m_name;
}

inline string_id Object::get_name_id() const
{
    return m_name_id;
}

inline const char * Object::get_type() const 
{
	return m_type;
}


inline string_id Object::get_type_id() const
{
    return m_type_id;
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
