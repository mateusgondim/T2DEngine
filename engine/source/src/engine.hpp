#ifndef _ENGINE_HPP
#define _ENGINE_HPP

#include <stdint.h>
class Tile_map;

void engine_init(const uint32_t context_version_major, const uint32_t context_version_minor,
                 const char * pplevels[], const int num_levels);
void engine_shut_down();

#endif // !_ENGINE_HPP
