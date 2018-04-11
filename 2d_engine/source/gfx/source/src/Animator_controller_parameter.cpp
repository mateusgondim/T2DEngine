#include "Animator_controller_parameter.hpp"
#include "string_id.hpp"

gfx::Animator_controller_parameter::Animator_controller_parameter() : m_type(Type::INT), m_default_int(0) {}

gfx::Animator_controller_parameter::Animator_controller_parameter(const string_id name_id, const Type type) : m_name_id(name_id), m_type(type)
{
	switch (m_type) {
	case Type::FLOAT:
		m_default_float = 0.0f;
		break;
	case Type::INT:
		m_default_int = 0;
		break;
	case Type::BOOL:
		m_default_bool = false;
		break;
	case Type::TRIGGER:
		m_default_bool = false;
		break;
	}
}

gfx::Animator_controller_parameter::Animator_controller_parameter(const Animator_controller_parameter & parameter) : m_name_id(parameter.m_name_id), m_type(parameter.m_type)
{
	switch (m_type) {
	case Type::FLOAT:
		m_default_float = parameter.m_default_float;
		break;
	case Type::INT:
		m_default_int = parameter.m_default_int;
		break;
	case Type::BOOL:
		m_default_bool = parameter.m_default_bool;
		break;
	case Type::TRIGGER:
		m_default_bool = parameter.m_default_bool;
		break;
	}
}

gfx::Animator_controller_parameter & gfx::Animator_controller_parameter::operator=(const Animator_controller_parameter & rhs) 
{
	m_name_id  =		rhs.m_name_id;
	m_type     =		rhs.m_type;

	switch (m_type) {
	case Type::FLOAT:
		m_default_float = rhs.m_default_float;
		break;
	case Type::INT:
		m_default_int = rhs.m_default_int;
		break;
	case Type::BOOL:
		m_default_bool = rhs.m_default_bool;
		break;
	case Type::TRIGGER:
		m_default_bool = rhs.m_default_bool;
		break;
	}

	return *this;
}

int gfx::Animator_controller_parameter::get_int() const 
{
	if (m_type == Type::INT) {
		return m_default_int;
	}
	return 0; //in case of error
}

float gfx::Animator_controller_parameter::get_float() const 
{
	if (m_type == Type::FLOAT) {
		return m_default_float;
	}
	return 0.0f;
}

bool gfx::Animator_controller_parameter::get_bool() const 
{
	if ( (m_type == Type::BOOL ) || (m_type == Type::TRIGGER) ) {
		return m_default_bool;
	}
	return false;
}

void gfx::Animator_controller_parameter::set_value(const float fvalue) 
{
	if (m_type == Type::FLOAT) {
		m_default_float = fvalue;
	}
}

void gfx::Animator_controller_parameter::set_value(const int ivalue) 
{
	if (m_type == Type::INT) {
		m_default_int = ivalue;
	}
}

void gfx::Animator_controller_parameter::set_value(const bool bvalue) 
{
	if ( (m_type == Type::BOOL) || (m_type == Type::TRIGGER)) {
		m_default_bool = bvalue;
	}
}