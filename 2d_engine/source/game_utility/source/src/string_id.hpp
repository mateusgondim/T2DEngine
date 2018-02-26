#ifndef _STRING_ID_HPP
#define _STRING_ID_HPP

#include <cstdint>

typedef std::uint32_t string_id;
extern  string_id intern_string(const char *str);

#endif // !_STRING_ID_HPP
