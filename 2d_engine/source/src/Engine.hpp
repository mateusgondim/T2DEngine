#ifndef _ENGINE_HPP
#define _ENGINE_HPP

#include <stdint.h>
class Tile_map;

void engine_init(const uint32_t context_version_major, const uint32_t context_version_minor, Tile_map *ptile_map);
void engine_shut_down();

#endif // !_ENGINE_HPP
