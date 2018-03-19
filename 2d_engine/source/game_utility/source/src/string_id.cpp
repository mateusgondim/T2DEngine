#include "string_id.hpp"
#include "crc32.hpp"
#include <map>
#include <cstdint>


static std::map<string_id, const char *> g_string_table;

string_id intern_string(const char *str) 
{
	string_id sid = get_crc32(str);

	std::map<string_id, const char*>::iterator iter = g_string_table.find(sid);
	if (iter == g_string_table.end()) {
		//the string is not saved on the table
		g_string_table[sid] = strdup(str);
	}

	return sid;
}

void	remove_string_id_entry(string_id id) 
{
	std::map<string_id, const char*>::iterator it = g_string_table.find(id);
	if (it != g_string_table.end()) {
		
		//deallocate string
		free((void*)it->second);
		
		//remove entry from the table
		g_string_table.erase(id);
	}
}