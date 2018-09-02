#include "Object_group.hpp"
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <iostream>

Object_group::Object_group() : m_name(nullptr) {}

Object_group::~Object_group() 
{
	if (m_name) {
		free(m_name);
	}

	std::for_each(m_objects.begin(), m_objects.end(), [](Object *pobject) {delete pobject; });
}

void Object_group::set_name(const char *str) 
{
	if (m_name) {
		free(m_name);
	}
	m_name = strdup(str);
}

std::ostream & operator<<(std::ostream & os, const Object_group & obj_group) 
{
	os << "<objectgroup name=\"" << obj_group.get_name() << "\">" << std::endl;

	const std::vector<Object*> & objects = obj_group.get_objects();
	for (std::vector<Object*>::const_iterator it = objects.cbegin(); it != objects.cend(); ++it) {
		os << " " << *(*it) << std::endl;
	}
	os << "</objectgroup>";

	return os;
}