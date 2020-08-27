#include "Object.hpp"

#include <cstdlib>
#include <cstring>

#include <iostream>

#include "Tile_map.hpp"

Object::Object()
    : m_id(0),
      m_x(0),
      m_y(0),
      m_width(0),
      m_height(0),
      m_gid(0),
      m_rotation(0.0f),
      m_name(nullptr),
      m_type(nullptr),
      m_properties(1, get_property_hash_code){}

Object::Object(const Object & obj)
    : m_id(obj.m_id),
      m_x(obj.m_x),
      m_y(obj.m_y),
      m_width(obj.m_width),
      m_height(obj.m_height),
      m_gid(obj.m_gid),
      m_rotation(obj.m_rotation),
      m_properties(obj.m_properties)
{
	if (obj.m_name) {
		m_name = strdup(obj.m_name);
	}
	if (obj.m_type) {
		m_type = strdup(obj.m_type);
	}
}

Object & Object::operator=(const Object & rhs) 
{
	m_id = rhs.m_id;
	m_x = rhs.m_x;
	m_y = rhs.m_y;
	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_gid = rhs.m_gid;
	m_rotation = rhs.m_rotation;

	if (m_name) {
		free(m_name);
	}
	
	if (m_type) {
		free(m_type);
	}
	
	if (rhs.m_name) {
		m_name = strdup(rhs.m_name);
	}

	if (rhs.m_type) {
		m_type = strdup(rhs.m_type);
	}

	return *this;
}

void Object::set_name(const char * pname) 
{
	if (m_name) {
		free(m_name);
	}
	m_name = strdup(pname);
}

void Object::set_name_id(const string_id name_id)
{
    m_name_id = name_id;
}

void Object::set_type_id(const string_id type_id)
{
    m_type_id = type_id;
}

void Object::set_width(const float width)
{
    m_width = width;
}

void Object::set_height(const float heigth)
{
    m_height = heigth;
}


void Object::set_type(const char * ptype) 
{
	if (m_type) {
		free(m_type);
	}
	m_type = strdup(ptype);
}

Object::~Object() 
{
	if (m_name) {
		free(m_name);
	}
	if (m_type) {
		free(m_type);
	}
}

const Property * Object::get_property(const string_id name_id) const
{
    Property p(name_id);
    Property_unordered_set::const_iterator iter = m_properties.find(p);
    return (iter == m_properties.cend()) ? (nullptr) : (&(*iter));
}

std::ostream & operator<<(std::ostream & os, const Object & object) 
{
	os << "<object id=\"" << object.m_id << "\"";
	if (object.m_name) {
		os << " name=\"" << object.m_name << "\"";
	}
	if (object.m_type) {
		os << " type=\"" << object.m_type << "\"";
	}
	if (object.m_gid) {
		os << " gid=\"" << object.m_gid << "\"";
	}
	os << " x=\"" << object.m_x << "\"" << " y=\"" << object.m_y << "\"";

	if (object.m_width) {
		//if width is not zero, than height must not be zero either
		os << " width=\"" << object.m_width << "\"" << " height=\"" << object.m_height << "\"";
	}

	if (object.m_rotation) {
		os << " rotation=\"" << object.m_rotation << "\"";
	}

	//check if it has properties

	if (!object.m_properties.empty()) {
		os << ">" << std::endl;
		os << "  <properties>" << std::endl;
		for (Object::property_set::iterator it = object.m_properties.cbegin(); it != object.m_properties.cend(); ++it) {
			os << "\t" << *it << std::endl;
		}
		os << "  </properties>" << std::endl;
		os << "</object>";
	}
	else {
		os << "/>";
	}

	return os;
}