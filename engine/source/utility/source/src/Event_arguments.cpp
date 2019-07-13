#include "Event_arguments.hpp"
#include "Variant.hpp"
#include <stdint.h>
#include <cassert>
#include <iostream>

constexpr std::size_t   Event_arguments::s_table_size;

Variant & Event_arguments::operator[](const key_type key)
{
        key_type index = find_key_or_index(key);
        assert(index != -1);
        if (m_hash_table[index].key == -1) {
                m_hash_table[index].key = key;
                ++m_length;
        }
        return m_hash_table[index].value;
}

const Variant * Event_arguments::find(const key_type key) const
{
        Event_arguments::key_type index = Event_arguments::find_key_or_index(key);
        if (index == -1) {
                return nullptr;
        }
        const Argument *pargument = &m_hash_table[index];
        return (pargument->key == key) ?(&pargument->value) :(nullptr);
}

const Variant * Event_arguments::insert(const key_type key, const Variant & value)
{
        Event_arguments::key_type index = Event_arguments::find_key_or_index(key);
        if (index == -1) {
                return nullptr;
        }
        if (m_hash_table[index].key == -1) { // element is not in the table
                m_hash_table[index].key = key;
                m_hash_table[index].value = value;
                m_length++;
        }
        else {
                m_hash_table[index].value = value;
        }
        return &m_hash_table[index].value;
}

/**
 * Probes the table to try to find the element with key 'key' or, the index
 * in the table to store the key
 *
 * @params key is the key that is being searched
 * @return The index of the element in the table that contains the key or,
 * the index in which this element with key 'key' would be stored or, -1 in 
 * case that the key does not exist and the table is full.
 */
Event_arguments::key_type Event_arguments::find_key_or_index(const key_type key) const
{
        int index = Event_arguments::index_from_value(key);
        const Argument *pargument = nullptr;
#ifndef NDEBUG
        std::cout << __FUNCTION__ << ": Trying to find key '" << std::hex << key
                << "', starting at index '" << index << "'" << std::endl;
#endif // !NDEBUG

        pargument = &m_hash_table[index];
        if (pargument->key == key || pargument->key == -1) {
                return index;
        }

        int i = 0;
        for (uint32_t j = 1; j != Event_arguments::s_table_size; ++j) {
                i = Event_arguments::index_from_value(index + j * j);
                pargument = &m_hash_table[i];
#ifndef NDEBUG
                std::cout << "Probing at index: " << i << std::endl;
#endif // !NDEBUG

                if (pargument->key == key || pargument->key == -1) {
                        return i;
                }

                int displacement = Event_arguments::index_from_value(j*j);
                i = index - displacement;
                i = (i < 0) ? (i + Event_arguments::s_table_size) : (i);
                pargument = &m_hash_table[i];
#ifndef NDEBUG
                std::cout << "Probing at index: " << i << std::endl;
#endif // !NDEBUG
                if (pargument->key == key || pargument->key == -1) {
                        return i;
                }
        }
        return -1; // key does not exist and there is no place to insert
}

