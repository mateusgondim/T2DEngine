#include "Tile_map.hpp"

#include <stdint.h>
#include <utility>
#include <string>
#include <fstream>
#include <algorithm>

#include "Property.hpp"
#include "Tile.hpp"
#include "Tileset.hpp"
#include "Tile_layer.hpp"
#include "Object.hpp"
#include "Object_group.hpp"
#include "Rect.hpp"

/*
parse map header/element
        parse map's variables
        while not </map>
                while <tileset>
                        parse tileset
                end
                while <layer> or <objectgroup>
                        if <layer>
                                parse <layer>
                        elseif <objectgroup>
                                parse <objectgroup>
                        end
                end
        end
*/

static const Tile g_empty_tile;

Tile_map::Tile_map(const std::string & tmx_file_path)
{
        std::ifstream file_istream;
        std::string   line;

        file_istream.open(tmx_file_path);
        if (file_istream) {
                std::getline(file_istream, line);
#ifndef NDEBUG
                std::cout << "xml prolog: " << line << std::endl;
#endif // !NDEBUG
                //TODO: check enconding

                std::getline(file_istream, line);
                // process map header
#ifndef NDEBUG
                std::cout << "map header: " << line << std::endl;
#endif // !NDEBUG
                parse_map_element_variables(line);

                // parse the tilesets
           
                while ((std::getline(file_istream, line)) && (line.find("<tileset") != std::string::npos)) {
                        parse_tileset(tmx_file_path, line);
                }

                //Parse layers
		std::string digits("0123456789");
                while ((file_istream) &&  ( (line.find("<layer") != std::string::npos) || (line.find("<objectgroup") != std::string::npos) ) )  {
			if (line.find("<layer") != std::string::npos) {
				Tile_layer *ptile_layer = new Tile_layer;

				auto pos = line.find("name");
				pos = line.find_first_of("\"", pos);
				++pos;
				std::string layer_name = line.substr(pos, line.find_first_of("\"", pos) - pos);
				ptile_layer->set_name(layer_name.c_str());

				pos = line.find("width");
				int layer_width = std::stoi(line.substr(line.find_first_of(digits, pos)));
				ptile_layer->m_width_in_tiles = layer_width;

				pos = line.find("height");
				int layer_height = std::stoi(line.substr(line.find_first_of(digits, pos)));
				ptile_layer->m_height_in_tiles = layer_height;

				//TODO get aditional Tile_layer's members


				//read the encoding line
				std::getline(file_istream, line);
#ifndef NDEBUG
				std::cout << "Layer " << layer_name << " data enconding header: " << line << std::endl;
#endif // !NDEBUG
				//Parse the data

				for (int i = 0; i < layer_height; ++i) { // for each row of data
					std::getline(file_istream, line); // get row

					auto next_number = line.find_first_of(digits);
					for (int j = 0; j < layer_width; ++j) { // for each column(element) on the current row
						ptile_layer->m_data.push_back(std::stoi(line.substr(next_number)));
						next_number = line.find_first_of(",", next_number);
						++next_number;
					}
				}
				m_layers.push_back(ptile_layer);

				std::getline(file_istream, line); // the closing </data>
				std::getline(file_istream, line); // the closing </layer>
				std::getline(file_istream, line); // get the next <layer>, <objectgroup> or </map>
			}
			else if (line.find("<objectgroup") != std::string::npos) {  //Parse <objectgroup
				Object_group *pobj_group = new Object_group();

				auto pos = line.find("name");
				if (pos != std::string::npos) {
					pos = line.find_first_of("\"", pos);
					++pos;
					std::string layer_name = line.substr(pos, line.find_first_of("\"", pos) - pos);
					pobj_group->set_name(layer_name.c_str());
				}

				//parse each <object> element of the <objectgroup>
				std::getline(file_istream, line);
				while (file_istream && (line.find("<object") != std::string::npos)) {
					Object *pobj = new Object();
					
					//id
					pos = line.find("id=");
					uint32_t obj_id = std::stoi(line.substr(line.find_first_of(digits, pos)));
					pobj->m_id = obj_id;

					//name
					pos = line.find("name=");
					if (pos != std::string::npos) {
						pos = line.find_first_of("\"", pos);
						++pos;
						std::string obj_name = line.substr(pos, line.find_first_of("\"", pos) - pos);
						pobj->set_name(obj_name.c_str());
					}

					//type
					pos = line.find("type=");
					if (pos != std::string::npos) {
						pos = line.find_first_of("\"", pos);
						++pos;
						std::string obj_type = line.substr(pos, line.find_first_of("\"", pos) - pos);
						pobj->set_type(obj_type.c_str());
					}

					pos = line.find("gid=");
					if (pos != std::string::npos) {
						pobj->m_gid = std::stoi(line.substr(line.find_first_of(digits, pos)));
					}

					pos = line.find("x=");
					pobj->m_x = std::stof(line.substr(line.find_first_of(digits, pos)));

					pos = line.find("y=");
					pobj->m_y = std::stof(line.substr(line.find_first_of(digits, pos)));

					pos = line.find("width=");
					if (pos != std::string::npos) {
						pobj->m_width = std::stof(line.substr(line.find_first_of(digits, pos)));

						pos = line.find("height=");
						pobj->m_height = std::stof(line.substr(line.find_first_of(digits, pos)));
					}

					pos = line.find("rotation=");
					if (pos != std::string::npos) {
						pobj->m_rotation = std::stof(line.substr(line.find_first_of(digits, pos)));
					}

					//parse <properties> , <point> , <polyline>, or get the enclosing </object> line
					std::getline(file_istream, line);
					
					if (file_istream && (line.find("<properties>") != std::string::npos)) {
						//parse each <property> element
						std::getline(file_istream, line);
						while (file_istream && (line.find("<property") != std::string::npos)) {
							Property p(line);
							pobj->m_properties.insert(std::move(p));
							std::getline(file_istream, line);
						}

						//get the line after </properties>
						std::getline(file_istream, line);
					}

					if (file_istream && (line.find("<point/>") != std::string::npos)) {
						std::getline(file_istream, line);
					}

					if (file_istream && (line.find("<polyline") != std::string::npos)) { //parse <polyline>
						std::getline(file_istream, line);
					}

					if (file_istream && (line.find("</object>") != std::string::npos)) { //get the next <object> element 
						std::getline(file_istream, line);
					}
					
					pobj_group->m_objects.push_back(pobj);
				}
				m_object_layer.push_back(pobj_group);
				//get the next line after </object_group>
				std::getline(file_istream, line);
			}
			//std::getline(file_istream, line); // next <layer>, <objectgroup or enclosing </map>
                }
                
		//tile_map = Tile_map(tilesets, layers, layer_count, width, height, tile_width, tile_height);
		
		//read the </map> line, finished parsing
                file_istream.close();
        }
        else {
                std::cerr << "ERROR: could not open file: " << tmx_file_path << std::endl;
        }
}


void Tile_map::parse_map_element_variables(const std::string & line) 
{
        // parse map header
        std::string digits("0123456789");

        //get map's width in tiles
        auto pos_aux = line.find("width", 0);
        int width = std::stoi(line.substr(line.find_first_of(digits, pos_aux)));

        //get map's height in tiles
        pos_aux = line.find("height", 0);
        int height = std::stoi(line.substr(line.find_first_of(digits, pos_aux)));

        pos_aux = line.find("tilewidth", 0);
        int tile_width = std::stoi(line.substr(line.find_first_of(digits, pos_aux)));

        pos_aux = line.find("tileheight");
        int tile_height = std::stoi(line.substr(line.find_first_of(digits, pos_aux)));

        //check if there is background color
        pos_aux = line.find("backgroundcolor");
        math::vec3 background_color(0.0f, 0.0f, 0.0f);
        if (pos_aux != std::string::npos) {
                std::string r, g, b;
                pos_aux = line.find_first_of("#", pos_aux);
                ++pos_aux;
                //each next two caracters is an hexadecimal number
                r = line.substr(pos_aux, 2);
                pos_aux += 2;
                g = line.substr(pos_aux, 2);
                pos_aux += 2;
                b = line.substr(pos_aux, 2);
                background_color.x = std::stoi(r, nullptr, 16);
                background_color.y = std::stoi(g, nullptr, 16);
                background_color.z = std::stoi(b, nullptr, 16);

                //normalize it 
                m_background_color = background_color / 255.0f;
        }
        
        m_width = width;
        m_height = height;
        m_tile_width = tile_width;
        m_tile_height = tile_height;

        m_pixels_per_word_unit = m_tile_width;
}

void Tile_map::parse_tileset(const std::string & tmx_file_path, const std::string & first_line)
{
        //get the tileset's first global id
        auto pos = first_line.find("firstgid");
        pos = first_line.find_first_of("\"", pos);
        ++pos;
        int firstgid = std::stoi(first_line.substr(pos));

        //get the tileset's file path
        pos = first_line.find("source");
        pos = first_line.find_first_of("\"", pos);
        ++pos;
        std::string tileset_path = first_line.substr(pos, first_line.find_first_of("\"", pos) - pos);

#ifndef NDEBUG
        std::cout << "tileset first global id: " << firstgid << std::endl;
        std::cout << "tileset relative file path: " << tileset_path << std::endl;
#endif // !NDEBUG

        //get the full path from user input directory to the tileset path
        tileset_path = tileset_path.substr(tileset_path.find_first_of("/")); // remove the '..'

        std::string aux;
        if ((pos = tmx_file_path.find_last_of("\\")) != std::string::npos) { /* path is in format c: \a \b \ */
                aux = tmx_file_path.substr(0, pos);
                pos = aux.find_last_of("\\");
        }
        else { // path is in format      c:/a/b/c
                pos = tmx_file_path.find_last_of("/");
                aux = tmx_file_path.substr(0, pos);
                pos = aux.find_last_of("/");
        }
        tileset_path = aux.replace(pos, aux.size() - pos, tileset_path);

#ifndef NDEBUG
        std::cout << "Tileset full tsx file path: " << tileset_path << std::endl;
        std::cout << "Parse Tileset file..." << std::endl;
#endif // !NDEBUG

        //Parse the tileset's tsx file
        std::ifstream in;
        std::string   line;
        in.open(tileset_path);
        if (in) {
                std::getline(in, line); 
#ifndef NDEBUG
                std::cout << "Tsx header: " << line << std::endl;
#endif
                Tileset *ptileset = new Tileset;
                ptileset->m_first_gid = firstgid;
                // Parse header, i.e <tileset .. > line
                std::getline(in, line);
#ifndef NDEBUG
                std::cout << "tilset header: " << line << std::endl;
#endif 
                auto pos = line.find("name");
                pos = line.find_first_of("\"", pos);
                ++pos;
                std::string name = line.substr(pos, first_line.find_first_of("\"", pos) - pos);
                ptileset->m_name = name;

                std::string digits("0123456789");

                pos = line.find("tilewidth");
                int tile_width = std::stoi(line.substr(line.find_first_of(digits, pos)));
                ptileset->m_tile_width = tile_width;

                pos = line.find("tileheight");
                int tile_height = std::stoi(line.substr(line.find_first_of(digits, pos)));
                ptileset->m_tile_height = tile_height;

                pos = line.find("spacing");
                int spacing = (pos != std::string::npos) ? (std::stoi(line.substr(line.find_first_of(digits, pos)))) : (0);
                ptileset->m_spacing = spacing;

                pos = line.find("margin");
                int margin = (pos != std::string::npos) ? (std::stoi(line.substr(line.find_first_of(digits, pos)))) : (0);
                ptileset->m_margin = margin;

                pos = line.find("tilecount");
                int tile_count = std::stoi(line.substr(line.find_first_of(digits, pos)));
                ptileset->m_tile_count = tile_count;

                pos = line.find("columns");
                int columns = std::stoi(line.substr(line.find_first_of(digits, pos)));
                ptileset->m_columns = columns;

                // Parse <image
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


                pos = (aux.find_last_of("/") == std::string::npos) ? (aux.find_last_of("\\")) : (aux.find_last_of("/"));
                ++pos;
                texture_file_path = aux.replace(pos, aux.size() - pos, texture_file_path);

#ifndef NDEBUG
                std::cout << "Full path to texture: " << texture_file_path << std::endl;
#endif // !NDEBUG
                ptileset->m_texture_file_path = texture_file_path;

                pos = line.find("width");
                int width = std::stoi(line.substr(line.find_first_of(digits, pos)));
                ptileset->m_set_width = width;

                pos = line.find("height");
                int height = std::stoi(line.substr(line.find_first_of(digits, pos)));
                ptileset->m_set_height = height;

                //Parse <tiles
                ptileset->m_ptiles = new Tile[tile_count];
                while (std::getline(in, line) && (line.find("<tile") != std::string::npos)) { // while there are still tiles to process...
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

                        // Parse <properties>
                        if (in && (line.find("<properties>") != std::string::npos)) {
                                while (std::getline(in, line) && (line.find("<property") != std::string::npos)) { // process each property associeated with this tile
					Property property(line);

					std::string name(property.get_name());

					//TODO: REMOVE THIS HARDCODED TILE's data members!
					if (name == "is_obstacle") {
						ptileset->m_ptiles[tile_id - firstgid].m_is_obstacle = property.get_bool();
					}
					else if (name == "is_ladder") {
						ptileset->m_ptiles[tile_id - firstgid].m_is_ladder = property.get_bool();
					}
					else if (name == "is_one_way") {
						ptileset->m_ptiles[tile_id - firstgid].m_is_one_way = property.get_bool();
					}

					ptileset->m_ptiles[tile_id - firstgid].m_properties.insert(std::move(property));

                                }
                                // READ </properties>
                                if (has_object_group) { // if it had a <objectgroup , read the enclosing header
                                        std::getline(in, line); //</objectgroup>
                                }
                                std::getline(in, line); //</tile>
                        }
                }
		m_tilesets.push_back(ptileset);
		//------------------------- Debug -------------------------------------------------//
                std::cout << "This tile set have " << tile_count << " tiles with id's in " << "[" << firstgid << "  " << firstgid + tile_count - 1 << "] " << std::endl;
                //for (std::size_t i = 0; i < tile_count; ++i) {
                //        std::cout << "Tile no " << i + firstgid << " has property  is_obstacle = ";
                //        std::cout << ((ptiles[i].m_is_obstacle) ? ("true") : ("false")) << std::endl;
               // }
                //--------------------------------------------------------------------------------//
                //string_id texture_id = intern_string(texture_file_path.c_str());
                //tileset = Tileset(ptiles, firstgid, texture_id, tile_width, tile_height, margin, spacing, tile_count, columns, width, height);
                //tileset = Tileset(ptiles, firstgid, texture_file_path, tile_width, tile_height, margin, spacing, tile_count, columns, width, height);
		
		//tileset = Tileset(ptiles, firstgid, texture_file_path, tile_width, tile_height, margin, spacing, tile_count, columns, width, height);
        }
        else {
                std::cerr << "ERROR in funcion" << __FUNCTION__ << " located in the file " << __FILE__ << " Line " << __LINE__ << " : Could not open file: " << tileset_path << std::endl;
        }
}

Tile_map::~Tile_map() 
{
	std::for_each(m_tilesets.begin(), m_tilesets.end(), [] (Tileset *ptileset) { delete ptileset; });
	std::for_each(m_layers.begin(), m_layers.end(), [](Tile_layer *ptile_layer) {delete ptile_layer; });
	std::for_each(m_object_layer.begin(), m_object_layer.end(), [](Object_group *pobject_layer) {delete pobject_layer; });
}


//TODO: THROW EXCEPTION OR CHANGE TO TILE* AND RETURN NULLPTR
const Tile * Tile_map::get_tile(uint32_t id) const 
{
	if (id == 0) {
                return &g_empty_tile;
	}

	for (auto iter = m_tilesets.begin(); iter != m_tilesets.end(); ++iter) {
		if ((*iter)->is_inside_set(id)) {
			return (*iter)->get_tile(id);
		}
	}
        return &g_empty_tile;
}

const Object_group * Tile_map::get_object_group(const char *name) const
{
	std::vector<Object_group*>::const_iterator it;
	for (it = m_object_layer.cbegin(); it != m_object_layer.cend(); ++it) {
		if (!strcmp(name, (*it)->get_name())) {
			return *it;
		}
	}
	return nullptr;
}
//TAKE INTO  CONSIDERATION THE PIXELS PER_WORLD UNIT
math::Rect Tile_map::tile_wld_space_bounds(const unsigned row, const unsigned column) const
{	
	math::vec3 map_space_coord(column * m_tile_width / m_pixels_per_word_unit, (m_height * m_tile_height / m_pixels_per_word_unit) - (row * m_tile_height / m_pixels_per_word_unit), 0.0f);
	math::vec3 wld_space_coord = map_space_coord + m_position;

	math::Rect rect;
	rect.width   =  m_tile_width / m_pixels_per_word_unit;
	rect.height  =  m_tile_height / m_pixels_per_word_unit;
	rect.x       =  wld_space_coord.x;
	rect.y       =  wld_space_coord.y;

	return rect;
}

//get the row and column from a point in world space
std::pair<float, float> Tile_map::wld_to_tile_space(const math::vec3 & pos) const
{
	/*
	// transform point to tile space
	math::vec3 p_tspace = pos - m_position;

	unsigned column = p_tspace.x / (m_tile_width / m_pixels_per_word_unit);
	unsigned row = (m_height * m_tile_height / m_pixels_per_word_unit) - ( p_tspace.y / (m_tile_height / m_pixels_per_word_unit));

	return { row, column };
	*/
	float rows = (m_height * m_tile_height / m_pixels_per_word_unit) - ((pos.y - m_position.y) / (m_tile_height / m_pixels_per_word_unit));
	float columns =  (pos.x - m_position.x) / (m_tile_width / m_pixels_per_word_unit);

	return{ rows, columns };
}

std::ostream & print_tile_map(std::ostream & os, const Tile_map & tile_map) 
{
	os << "<map " << "width=" << tile_map.m_width << " height=" << tile_map.m_height << " tilewidth=" << tile_map.m_tile_width << " tileheight=" << tile_map.m_tile_height << " >" << std::endl;

	std::for_each(tile_map.m_tilesets.cbegin(), tile_map.m_tilesets.cend(), [& os] (const Tileset *ptileset) {os << *ptileset << std::endl;; });
	std::for_each(tile_map.m_layers.cbegin(), tile_map.m_layers.cend(), [&os](const Tile_layer * ptile_layer) {os << *ptile_layer << std::endl; });
	std::for_each(tile_map.m_object_layer.cbegin(), tile_map.m_object_layer.cend(), [&os](const Object_group * pobject_layer) {os << *pobject_layer << std::endl; });
	os << "</map>";

	return os;
}