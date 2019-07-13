#ifndef _EVENT_HPP
#define _EVENT_HPP

#include <stdint.h>
#include "Variant.hpp"
#include "Event_arguments.hpp"

/**
 * An encapsulated function call, to allow a 
 * dynamically typed late-bound function call
 */
class Event {
public:
        Event(uint32_t type) : m_type(type) {}
        const Event_arguments & get_arguments() const { return m_arguments; }
        Event_arguments &       get_arguments() { return m_arguments; }
        uint32_t                get_type() const { return m_type; }
private:
        uint32_t                m_type;
        Event_arguments         m_arguments;
};
#endif // !_EVENT_HPP

