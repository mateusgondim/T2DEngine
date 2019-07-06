#ifndef _VARIANT_HPP
#define _VARIANT_HPP

#include <stdint.h>
#include <iostream>

/**
 * Data object capable of holding more than one type of data
 *
 * Can be used to store an integer, float, boolean or
 * unsigned value.
 */
struct Variant {
        Variant() : m_type(TYPE_INTEGER), m_as_integer(0) {}
        Variant(const Variant & v);
        Variant(int32_t ivalue);
        Variant(float   fvalue);
        Variant(bool    bvalue);
        Variant(uint32_t uivalue);

        Variant & operator=(const Variant & rhs);


        enum Type {
                TYPE_INTEGER,
                TYPE_FLOAT,
                TYPE_BOOL,
                TYPE_STRING_ID,
                TYPE_COUNT // number of unique types
        };

        Type m_type;

        union {
                int             m_as_integer;
                float           m_as_float;
                bool            m_as_bool;
                uint32_t        m_as_string_id;
        };
};

std::ostream & operator<<(std::ostream & os, const Variant & variant);

#endif // !_VARIANT_HPP
