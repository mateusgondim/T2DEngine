#ifndef _STRING_ID_HPP
#define _STRING_ID_HPP

#include <cstdint>

typedef std::uint32_t string_id;
extern  string_id intern_string(const char *str);
const   char      *get_string_by_id(const string_id id);
extern  void	  remove_string_id_entry(string_id id);

#endif // !_STRING_ID_HPP
