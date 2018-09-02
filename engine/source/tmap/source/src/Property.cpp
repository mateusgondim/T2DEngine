#include "Property.hpp"
#include <cstring>
#include <string>
#include <iostream>

Property::Property() : m_pname(nullptr), m_type(Property::Type::INVALID_PROPERTY){}

Property::Property(const char *pname, const Property::Type type) 
{
	m_pname  =  strdup(pname);
	m_type   =  type;
}

Property::Property(const std::string & element)  
{
        std::string name;
        auto pos = element.find("name");
        pos = element.find_first_of("\"", pos);
        ++pos;
        name = element.substr(pos, element.find_first_of("\"", pos) - pos);
        m_pname = strdup(name.c_str());

        std::string type;
        pos = element.find("type");
        pos = element.find_first_of("\"", pos);
        ++pos;
        type = element.substr(pos, element.find_first_of("\"", pos) - pos);

        std::string value;
        pos = element.find("value");
        pos = element.find_first_of("\"", pos);
        ++pos;
        value = element.substr(pos, element.find_first_of("\"", pos) - pos);

        if (type == "string") {
                m_type = Type::STRING_PROPERTY;
                m_strvalue = strdup(value.c_str());
        }
        else if (type == "float") {
                m_type = Type::FLOAT_PROPERTY;
                m_fvalue = std::stof(value);
        }
        else if (type == "int") {
                m_type = Type::INT_PROPERTY;
                m_ivalue = std::stoi(value.c_str());
        }
        else if (type == "bool") {
                m_type = Type::BOOL_PROPERTY;
                if (value == "true") {
                        m_bvalue = true;
                }
                else {
                        m_bvalue = false;
                }
        }
        else if (type == "file") {
                m_type = Type::FILE_PROPERTY;
                m_strvalue = strdup(value.c_str());
        }
        else {
                m_type = Type::INVALID_PROPERTY;
        }
}

Property::Property(const Property & p) 
{
	m_pname = strdup(p.m_pname);
	m_type = p.m_type;
	switch(p.m_type) {
		case Type::STRING_PROPERTY:
			m_strvalue = strdup(p.m_strvalue);
			break;
		case Type::FLOAT_PROPERTY:
			m_fvalue = p.m_fvalue;
			break;
		case Type::INT_PROPERTY:
			m_ivalue = p.m_ivalue;
			break;
		case Type::BOOL_PROPERTY:
			m_bvalue = p.m_bvalue;
			break;
		case Type::FILE_PROPERTY:
			m_strvalue = strdup(p.m_strvalue);
			break;
		default:
			break;
	}
}

Property::Property(Property && p) noexcept
{
	m_pname = p.m_pname;
	p.m_pname = nullptr;

	m_type  = p.m_type;

	switch (p.m_type) {
	case Type::STRING_PROPERTY:
		m_strvalue = p.m_strvalue;
		p.m_strvalue = nullptr;
		break;
	case Type::FLOAT_PROPERTY:
		m_fvalue = p.m_fvalue;
		break;
	case Type::INT_PROPERTY:
		m_ivalue = p.m_ivalue;
		break;
	case Type::BOOL_PROPERTY:
		m_bvalue = p.m_bvalue;
		break;
	case Type::FILE_PROPERTY:
		m_strvalue = p.m_strvalue;
		p.m_strvalue = nullptr;
		break;
	default:
		break;
	}
}



void Property::set_value(const char *str) 
{
	if (m_type == Type::STRING_PROPERTY || m_type == Type::FILE_PROPERTY) {
		if (m_strvalue != nullptr) {
			free(m_strvalue);
		}
		m_strvalue = strdup(str);
	}
}

void Property::set_value(const float fvalue) 
{
	if (m_type == Type::FLOAT_PROPERTY) {
		m_fvalue = fvalue;
	}
}

void Property::set_value(const int ivalue) 
{
	if (m_type == Type::INT_PROPERTY) {
		m_ivalue == ivalue;
	}
}

void Property::set_value(const bool bvalue) 
{
	if (m_type == Type::BOOL_PROPERTY) {
		m_bvalue = bvalue;
	}
}

bool operator<(const Property & lhs, const Property & rhs) 
{
	if (strcmp(lhs.m_pname, rhs.m_pname) < 0) {
		return true;
	}
	else {
		return false;
	}
}

Property::~Property() 
{
	if (m_type == Type::FILE_PROPERTY || m_type == Type::STRING_PROPERTY) {
		if (m_strvalue != nullptr) {
			free(m_strvalue);
		}
	}

	free(m_pname);
}

std::ostream & operator<<(std::ostream & os, const Property & p) 
{
	os << "<property name=\"" << p.get_name() << "\" type=\"";
	Property::Type t = p.get_type();
	switch (t) {
	case Property::Type::STRING_PROPERTY:
		os << "string\" value=\"" << p.get_str() << "\"/>";
		break;
	case Property::Type::FLOAT_PROPERTY:
		os << "float\" value=\"" << p.get_float() << "\"/>";
		break;
	case Property::Type::INT_PROPERTY:
		os << "int\" value=\"" << p.get_int() << "\"/>";
		break;
	case Property::Type::BOOL_PROPERTY:
		os << "bool\" value=\"" << p.get_bool() << "\"/>";
		break;
	case Property::Type::FILE_PROPERTY:
		os << "file\" value=\"" << p.get_str() << "\"/>";
			break;
	case Property::Type::COLOR_PROPERTY:
		os << "color\" value=\"" << "\"/>";
		break;
	case Property::Type::INVALID_PROPERTY:
		os << "invalid\" />";
		break;
	}
	return os;
}