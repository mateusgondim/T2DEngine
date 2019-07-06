#ifndef _EVENT_ARGUMENTS_HPP
#define _EVENT_ARGUMENTS_HPP

#include <stdint.h>
#include <cstddef>
#include "Variant.hpp"



/** A hash table of fixed size, to store event arguments
 *
 * This class manages a hash table, capable of associating unsigned
 * values, as keys, with Variant values. Providing basic operations
 * for searching and adding new key-value pairs.
 *
 */
class Event_arguments {
public:
        typedef uint32_t                key_type;
                                        Event_arguments() = default;
        const Variant *                 find(const key_type key) const;
        const Variant *                 insert(const key_type key, const Variant & value);
        Variant &                       operator[](const key_type key);
        std::size_t                     get_length() const { return m_length; }
private:
        static inline key_type          index_from_value(const key_type value);
        key_type                        find_key_or_index(const key_type key) const;
        static constexpr  std::size_t   s_table_size = 8;
        std::size_t                     m_length = 0;
        struct Argument { 
                Variant         value;
                uint32_t        key = -1;
        };

        Argument                        m_hash_table[s_table_size];
};

inline Event_arguments::key_type Event_arguments::index_from_value(const key_type key)
{
        return key & (s_table_size - 1);
}

#endif // !_EVENT_ARGUMENTS_HPP
