#ifndef _CRC32_HPP
#define _CRC32_HPP

#include <cstdint>

extern std::uint32_t crc_table[256];
std::uint32_t get_crc32(const char *pword);

#endif // !_CRC32_HPP
