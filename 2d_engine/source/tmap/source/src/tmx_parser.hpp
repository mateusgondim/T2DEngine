#ifndef _TMX_PARSER_HPP
#define _TMX_PARSER_HPP	

#include <string>

class Tileset;
class Tile_map;

int load_tile_map(const std::string & path, Tile_map & tile_map);
int load_tileset(const std::string & map_file_dir, const std::string & header, Tileset & tileset);
#endif
