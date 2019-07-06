#include "Variant.hpp"
#include <stdint.h>
#include <iostream>

Variant::Variant(int32_t ivalue) : m_type(TYPE_INTEGER), m_as_integer(ivalue) {}

Variant::Variant(float fvalue) : m_type(TYPE_FLOAT), m_as_float(fvalue) {}

Variant::Variant(bool bvalue) : m_type(TYPE_BOOL), m_as_bool(bvalue) {}

Variant::Variant(uint32_t uivalue) : m_type(TYPE_STRING_ID), m_as_string_id(uivalue) {}

Variant & Variant::operator=(const Variant & rhs)
{
        switch (rhs.m_type) {
        case TYPE_INTEGER:
                this->m_type = TYPE_INTEGER;
                this->m_as_integer = rhs.m_as_integer;
                break;
        case TYPE_FLOAT:
                this->m_type = TYPE_FLOAT;
                this->m_as_float = rhs.m_as_float;
                break;
        case TYPE_BOOL:
                this->m_type = TYPE_BOOL;
                this->m_as_bool = rhs.m_as_bool;
                break;
        case TYPE_STRING_ID:
                this->m_type = TYPE_STRING_ID;
                this->m_as_string_id = rhs.m_as_string_id;
                break;
        }
        return *this;
}

std::ostream & operator<<(std::ostream & os, const Variant & variant)
{
        switch (variant.m_type) {
        case Variant::Type::TYPE_INTEGER:
                os << variant.m_as_integer;
                break;
        case Variant::Type::TYPE_FLOAT:
                os << variant.m_as_float;
                break;
        case Variant::Type::TYPE_STRING_ID:
                os << variant.m_as_string_id;
                break;
                case Variant::Type::TYPE_BOOL:
                        if (variant.m_as_bool) {
                                os << "true";
                        }
                        else {
                                os << "false";
                        }
                        break;
        }
        return os;
}