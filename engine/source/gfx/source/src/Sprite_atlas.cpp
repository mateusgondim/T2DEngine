#include "Sprite_atlas.hpp"
#include "Resource.hpp"
#include "Resource_manager.hpp"
#include "Texture_2d.hpp"
#include "string_id.hpp"
#include "crc32.hpp"
#include "Rect.hpp"

#include <string>
#include <utility>
#include <iostream>
#include <fstream>


gfx::Sprite_atlas::Sprite_atlas(rms::Resource_manager *pcreator, const char *name) : 
	rms::Resource(pcreator, name), m_atlas_file_path(nullptr), m_texture_file_path(nullptr) {}

void gfx::Sprite_atlas::set_atlas_file_path(const char *file_path) 
{
	if (m_atlas_file_path != nullptr) {
		free(m_atlas_file_path);
	}
	m_atlas_file_path = strdup(file_path);
}

const char *gfx::Sprite_atlas::get_texture_file_path() const { return m_texture_file_path; }

gfx::Texture_2d *gfx::Sprite_atlas::get_texture() { return m_ptexture; }

void gfx::Sprite_atlas::load() 
{
	//process the tileset xml file
	std::ifstream	in;
	std::string		line;

	in.open(m_atlas_file_path);
	if (in) {
		std::getline(in, line); // xml header
#ifndef NDEBUG
		std::cout << "xml header: " << line << std::endl;
#endif // !NDEBUG
		while (std::getline(in, line) && (line.find("<TextureAtlas") == std::string::npos)) { //parse until find the <TextureAtlas line
			;
		}
		if (!in) { //did not find the <TextureAtlas line
			std::cerr << __FUNCTION__ << ": Error parsing file " << m_atlas_file_path << ", could not find <TextureAtlas header" << std::endl;
		}
		else {
#ifndef NDEBUG
			std::cout << "Parsing atlas:" << std::endl << line << std::endl;
#endif // !NDEBUG
			//get the texture atlas file name 
			std::string	texture_file;
			std::string	aux;
			auto pos = line.find("imagePath");
			pos = line.find_first_of("\"", pos);
			++pos;
			texture_file = line.substr(pos, line.find_first_of("\"", pos) - pos);
			//concatenate the name with the currend directorie to have the full path 
			aux = m_atlas_file_path;
			pos = (aux.find_last_of("/") == std::string::npos) ? (aux.find_last_of("\\")) : (aux.find_last_of("/"));
			++pos;
			texture_file = aux.replace(pos, aux.size() - pos, texture_file);
#ifndef NDEBUG
			std::cout << __FILE__ << ": Full path to texture atlas: " << texture_file << std::endl;
#endif // !NDEBUG

			std::string digits("0123456789");
			//get atlas witdth and height
			pos = line.find("width");
			int texture_width = std::stoi(line.substr(line.find_first_of(digits, pos)));

			pos = line.find("height");
			int texture_height = std::stoi(line.substr(line.find_first_of(digits, pos)));

			//loop trough all the sprite lines
            std::string sprite_name;

			while (std::getline(in, line) && (line.find("<sprite") != std::string::npos)) { // for every sprite int the atlas
                if (line.find("<!--") != std::string::npos) {
                    continue;
                }
				float x, y, w, h;
				//	std::cout << line << std::endl;
				pos = line.find("x");
				x = std::stoi(line.substr(line.find_first_of(digits, pos)));
				pos = line.find("y");
				y = std::stoi(line.substr(line.find_first_of(digits, pos)));
				pos = line.find("w", pos);
				w = std::stoi(line.substr(line.find_first_of(digits, pos)));;
				pos = line.find("h", pos);
				h = std::stoi(line.substr(line.find_first_of(digits, pos)));

                pos = line.find("n");
                pos = line.find_first_of('\"', pos);
                // closing comma
                auto last_pos = line.find_first_of("\"", pos + 1);
                sprite_name = line.substr(pos + 1, last_pos - (pos + 1));

                // Calculate the normalized texture coordinates
				math::Rect texture_coordinates(x/texture_width, y/texture_height,
                                               w/texture_width, h/texture_height);

                Atlas_image atlas_image;
                atlas_image.m_texture_coordinates = texture_coordinates;
                atlas_image.m_width = w;
                atlas_image.m_height = h;

                m_images.insert(std::make_pair(get_crc32(sprite_name.c_str()), atlas_image));
			}

			//set this atlas texture file path
			if (m_texture_file_path != nullptr) {
				free(m_texture_file_path);
			}

			m_texture_file_path = strdup(texture_file.c_str());
		}
	}
	else {
		std::cerr << __FUNCTION__ << ": could not open file " << m_atlas_file_path << std::endl;
	}
}

size_t gfx::Sprite_atlas::calculate_size() const 
{
	return sizeof(Sprite_atlas);
}

void gfx::Sprite_atlas::unload() 
{
	remove_string_id_entry(m_id);
	free(m_atlas_file_path);
	free(m_texture_file_path);
}


//get the uv coordinates for the sprite number 'sprite_no' in the texture atlas
const gfx::Atlas_image & gfx::Sprite_atlas::get_image_data(const string_id image_id) const
{
        std::map<string_id, Atlas_image>::const_iterator iter = m_images.find(image_id);
        if (iter != m_images.cend()) {
                return iter->second;
        }

        // if the texture is not found, return the coordinates for a
        // 'default error texture', such as a pink quad
        string_id error_texture_id = get_crc32("TEXTURE_NOT_FOUND");
        iter = m_images.find(error_texture_id);
        return iter->second;
}

/*
gfx::Sprite_atlas::~Sprite_atlas() 
{
	//erase the id of the atlas
	remove_string_id_entry(m_id);
}*/