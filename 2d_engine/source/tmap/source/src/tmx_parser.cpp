#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "tmx_parser.hpp"
//#include "string_id.hpp"
#include "Tile.hpp"
#include "Tileset.hpp"
#include "Tile_map.hpp"


int load_tileset(const std::string & map_file_dir ,const std::string & header, Tileset & tileset) 
{
	//get the tileset's first global id
	auto pos = header.find("firstgid");
	pos = header.find_first_of("\"", pos);
	++pos;
	int firstgid = std::stoi(header.substr(pos));
	
	//get the tileset's file path
	pos = header.find("source");
	pos = header.find_first_of("\"", pos);
	++pos;
	std::string tileset_path = header.substr(pos, header.find_first_of("\"", pos)  - pos );

#ifndef NDEBUG
	std::cout << "tileset first global id: " << firstgid << std::endl;
	std::cout << "tileset relative file path: " << tileset_path << std::endl;
#endif // !NDEBUG

	//get the path from user input directory to the tileset path
	tileset_path = tileset_path.substr(tileset_path.find_first_of("/")); // remove the '..'
	
	std::string aux;
	if ((pos = map_file_dir.find_last_of("\\")) != std::string::npos ) { /* path is in format c: \a \b \ */
		aux = map_file_dir.substr(0, pos);
		pos = aux.find_last_of("\\");
	}
	else { // path is in format      c:/a/b/c
		pos = map_file_dir.find_last_of("/");
		aux = map_file_dir.substr(0, pos);
		pos = aux.find_last_of("/");
	}
	tileset_path = aux.replace(pos, aux.size() - pos, tileset_path);

#ifndef NDEBUG
	std::cout << "Tileset full tsx file path: " << tileset_path << std::endl;
	std::cout << "Processing Tileset file..." << std::endl;
#endif // !NDEBUG

	//process the tileset tsx file
	std::ifstream in;
	std::string   line;
	in.open(tileset_path);
	if (in) {
		std::getline(in, line); // tsx header
#ifndef NDEBUG
		std::cout << "Tsx header: " << line << std::endl;
#endif // !NDEBUG

		// <tileset
		std::getline(in, line);
#ifndef NDEBUG
		std::cout << "tilset header: " << line << std::endl;
#endif // !NDEBUG
		std::string digits("0123456789");
		
		auto pos = line.find("tilewidth");
		int tile_width = std::stoi(line.substr(line.find_first_of(digits, pos)));

		pos = line.find("tileheight");
		int tile_height = std::stoi(line.substr(line.find_first_of(digits, pos)));

		pos = line.find("margin");
		int margin = (pos != std::string::npos) ?(std::stoi(line.substr(line.find_first_of(digits, pos)))) :(0);

		pos = line.find("spacing");
		int spacing = (pos != std::string::npos) ? (std::stoi(line.substr(line.find_first_of(digits, pos)))) : (0);

		pos = line.find("tilecount");
		int tile_count = std::stoi(line.substr(line.find_first_of(digits, pos)));

		pos = line.find("columns");
		int columns = std::stoi(line.substr(line.find_first_of(digits, pos)));

		// <image
		std::getline(in, line);
#ifndef NDEBUG
		std::cout << "image header: " << line << std::endl;
#endif // !NDEBUG

		// get the texture image file path
		std::string texture_file_path;
		std::string aux;
		pos = line.find("source");
		pos = line.find_first_of("\"", pos);
		++pos;
		texture_file_path = line.substr(pos, line.find_first_of("\"", pos) - pos);

		aux = tileset_path;
		

		pos = (aux.find_last_of("/") == std::string::npos) ? (aux.find_last_of("\\")) :(aux.find_last_of("/"));
		++pos;
		texture_file_path = aux.replace(pos, aux.size() - pos ,texture_file_path);

#ifndef NDEBUG
		std::cout << "Full path to texture: " << texture_file_path << std::endl;
#endif // !NDEBUG

		pos = line.find("width");
		int width  = std::stoi(line.substr(line.find_first_of(digits, pos)));

		pos = line.find("height");
		int height = std::stoi(line.substr(line.find_first_of(digits, pos)));


		Tile * ptiles = new Tile[tile_count];

		//process <tiles 
		while (std::getline(in, line) && (line.find("<tile") != std::string::npos ) ) { // while there are still tiles to process...
#ifndef NDEBUG
			std::cout << "Tile header " << line << std::endl;
#endif // !NDEBUG
			bool has_object_group;
			int tile_id;
			pos = line.find("id");
			tile_id = std::stoi(line.substr(line.find_first_of(digits, pos))); // get the tile's id

			++tile_id;  // the tileset file assumes the id starts at firstgid - 1, so we need to offset it

			std::getline(in, line); // <object group
			if (line.find("<objectgroup") != std::string::npos) {
				has_object_group = true;
				pos = line.find("draworder");
				pos = line.find_first_of("\"", pos);
				++pos;
				std::string draw_order = line.substr(pos, line.find_first_of("\"", pos) - pos);
#ifndef NDEBUG
				std::cout << __FUNCTION__ << "Tile with id = " << tile_id << " has a draworder= " << draw_order << std::endl;
#endif // !NDEBUG

			}
			else {
				has_object_group = false;
				std::cerr << __FUNCTION__ << ": no <objectgroup header for tile with id = " << tile_id << std::endl;
			}
			if (has_object_group) { //if it had a <objectgroup, get the next line
				std::getline(in, line);
			}
	
			// <properties>
			if (in && (line.find("<properties>") != std::string::npos)) {
				while (std::getline(in, line) && (line.find("<property") != std::string::npos)) { // process each property associeated with this tile
					std::string property_name, property_type;
					pos = line.find("name");
					pos = line.find_first_of("\"", pos);
					++pos;
					property_name = line.substr(pos, line.find_first_of("\"", pos) - pos);

					pos = line.find("type");
					pos = line.find_first_of("\"", pos);
					++pos;
					property_type = line.substr(pos, line.find_first_of("\"", pos) - pos);

					//read the appropriate type
					if (property_type == "bool") {  // bool property
						bool bvalue;
						std::string strvalue;

						pos = line.find("value");
						pos = line.find_first_of("\"", pos);
						++pos;
						strvalue = line.substr(pos, line.find_first_of("\"", pos) - pos);
						if (strvalue == "true") {
							bvalue = true;
						}
						else {
							bvalue = false;
						}
					
						//find the property to save
						if (property_name == "is_obstacle") {
							ptiles[tile_id - firstgid].m_is_obstacle = bvalue;
						}
						else if (property_name == "is_ladder") {
							ptiles[tile_id - firstgid].m_is_ladder = bvalue;
						}
						else if (property_name == "is_one_way") {
							ptiles[tile_id - firstgid].m_is_one_way = bvalue;
						}
						else {
							std::cerr << __FUNCTION__ << ": Unknow tile no." << tile_id <<  " bool property ' " << property_name << "'" << std::endl;
						}
					}
					else if (property_type == "int") { // int property
						int ivalue;
						pos = line.find("value");
						ivalue = std::stoi(line.substr(line.find_first_of(digits, pos)));
					}
					else if (property_type == "float") { // float property
						float fvalue;
						pos = line.find("value");
						fvalue = std::stof(line.substr(line.find_first_of(digits, pos)));
					}
					else if (property_type == "string") {   // string property
						std::string svalue;
						pos = line.find("value");
						pos = line.find_first_of("\"", pos);
						++pos;
						svalue = line.substr(pos, line.find_first_of("\"", pos) - pos);
					}
					else {
						std::cerr << __FUNCTION__ << ": Unknow property type ' " << property_type << "' for tile no. " << tile_id << std::endl;
					}
				}
				// READ </properties>
				if (has_object_group) { // if it had a <objectgroup , read the enclosing header
					std::getline(in, line); //</objectgroup>
				}
				std::getline(in, line); //</tile>
			}
		}
		//------------------------- Debug -------------------------------------------------//
		std::cout << "This tile set have " << tile_count << " tiles with id's in " << "[" << firstgid << "  " << firstgid + tile_count - 1 << "] " << std::endl;
		for (std::size_t i = 0; i < tile_count; ++i) {
			std::cout << "Tile no " << i + firstgid << " has property  is_obstacle = ";
			std::cout << ( (ptiles[i].m_is_obstacle) ? ("true") :("false") ) << std::endl;
		}
		//--------------------------------------------------------------------------------//
		//string_id texture_id = intern_string(texture_file_path.c_str());
		//tileset = Tileset(ptiles, firstgid, texture_id, tile_width, tile_height, margin, spacing, tile_count, columns, width, height);
		//tileset = Tileset(ptiles, firstgid, texture_file_path, tile_width, tile_height, margin, spacing, tile_count, columns, width, height);
		tileset = Tileset(ptiles, firstgid, texture_file_path, tile_width, tile_height, margin, spacing, tile_count, columns, width, height);

		return 0;
	}
	else {
		std::cerr << "ERROR in funcion" << __FUNCTION__ << " located in the file " << __FILE__ << " Line " << __LINE__ << " : Could not open file: " << tileset_path << std::endl;
		return -1;
	}

}

int load_tile_map(const std::string & path, Tile_map & tile_map) 
{
	std::ifstream file_istream;
	std::string   line;

	file_istream.open(path);
	if (file_istream) {
		std::getline(file_istream, line);
#ifndef NDEBUG
		std::cout << "header of the tmx file: " << line << std::endl;
#endif // !NDEBUG
		//TODO: check enconding
		
		
		std::getline(file_istream, line);
		// process map header
#ifndef NDEBUG
		std::cout << "map header: " << line << std::endl;
#endif // !NDEBUG
		std::string digits("0123456789");

		//get map's width in tiles
		auto pos_aux = line.find("width", 0);
		int width = std::stoi(line.substr(line.find_first_of(digits, pos_aux)));
		
		//get map's height in tiles
		pos_aux = line.find("height", 0);
		int height = std::stoi(line.substr(line.find_first_of(digits, pos_aux)));

		pos_aux = line.find("tilewidth", 0);
		int tile_width  = std::stoi(line.substr(line.find_first_of(digits, pos_aux)));

		pos_aux = line.find("tileheight");
		int tile_height = std::stoi(line.substr(line.find_first_of(digits, pos_aux)));

		// process the tilesets
		std::vector<Tileset> tilesets;
		while ( (std::getline(file_istream, line)) && (line.find("<tileset") != std::string::npos)) {
			Tileset tileset;
			if (!load_tileset(path, line, tileset)) {
				tilesets.push_back(tileset);
			}
			else {
				return -1;
			}
		}
		
		//process layers
		int layer_count = 0;
		std::vector<std::vector<std::vector<int>>> layers;
		while ( (file_istream) && (line.find("<layer") != std::string::npos )) {
			++layer_count;
#ifndef NDEBUG
			std::cout << "Layer number " << layer_count << " header: " << line << std::endl;
#endif // !NDEBUG

			auto pos = line.find("name");
			pos = line.find_first_of("\"", pos);
			++pos;
			std::string layer_name = line.substr(pos, line.find_first_of("\"", pos) - pos);

			pos = line.find("width");
			int layer_width = std::stoi(line.substr(line.find_first_of(digits, pos)));

			pos = line.find("height");
			int layer_height = std::stoi(line.substr(line.find_first_of(digits, pos)));

			//read the encoding line
			std::getline(file_istream, line);
#ifndef NDEBUG
			std::cout << "Layer " << layer_name << " data enconding header: " << line << std::endl;
#endif // !NDEBUG
			
			//process the data
			std::vector<std::vector<int>> layer(layer_height, std::vector<int>(layer_width));
			for (int i = 0; i < layer_height; ++i) { // for each row of data
				std::getline(file_istream, line); // get row
				
				auto next_number = line.find_first_of(digits);
				for (int j = 0; j < layer_width; ++j) { // for each column(element) on the current row
					layer[i][j] = std::stoi(line.substr(next_number));
					next_number = line.find_first_of(",", next_number);
					++next_number;
				}
			}
			layers.push_back(layer);

			std::getline(file_istream, line); // the closing </data>
			std::getline(file_istream, line); // the closing </layer>
			std::getline(file_istream, line); // next <layer> or enclosing </map>

		}
		tile_map = Tile_map(tilesets, layers, layer_count, width, height, tile_width, tile_height);

		file_istream.close();

		return 0;
		
	}
	else {
		std::cerr << "ERROR: could not open file: " << path << std::endl;
		return -1;
	}
}
