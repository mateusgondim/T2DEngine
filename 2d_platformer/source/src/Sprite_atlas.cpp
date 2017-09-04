#include "Sprite_atlas.hpp"
#include "Texture_2d.hpp"
#include "vec2.hpp"
#include "Rect.hpp"

#include <string>
#include <iostream>
#include <fstream>


/* Reads the xml file containing the coordinates of the sprites in the atlas and 
 * the directory of the image file and, loads the texture object and the sprite atlas data
 */
tgs::Sprite_atlas::Sprite_atlas(const std::string & file_name, const float pixels_per_unit) : m_file_path(file_name), m_pixels_per_unit(pixels_per_unit)
{

	//process the tileset xml file
	std::ifstream	in;
	std::string		line;

	in.open(file_name);
	if (in) {
		std::getline(in, line); // xml header
#ifndef NDEBUG
		std::cout << "xml header: " << line << std::endl;
#endif // !NDEBUG
		while (std::getline(in, line) && (line.find("<TextureAtlas") == std::string::npos)) { //parse until find the <TextureAtlas line
			;
		}
		if (!in) { //did not find the <TextureAtlas line
			std::cerr << __FUNCTION__ << ": Error parsing file " << file_name << ", could not find <TextureAtlas header" << std::endl;
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
			aux = file_name;
			pos = (aux.find_last_of("/") == std::string::npos) ? (aux.find_last_of("\\")) : (aux.find_last_of("/"));
			++pos;
			texture_file = aux.replace(pos, aux.size() - pos, texture_file);
#ifndef NDEBUG
			std::cout << __FILE__ << ": Full path to texture atlas: " << texture_file << std::endl;
#endif // !NDEBUG

			std::string digits("0123456789");
			//get atlas witdth and height
			pos = line.find("width");
			m_image_size.x = std::stoi(line.substr(line.find_first_of(digits, pos)));

			pos = line.find("height");
			m_image_size.y = std::stoi(line.substr(line.find_first_of(digits, pos)));

			//loop trough all the sprite lines
			while (std::getline(in, line) && (line.find("<sprite") != std::string::npos)) { // for every sprite int the atlas
				float x, y, w, h;

				pos = line.find("x");
				x = std::stoi(line.substr(line.find_first_of(digits, pos)));
				pos = line.find("y");
				y = std::stoi(line.substr(line.find_first_of(digits, pos)));
				pos = line.find("w");
				w = std::stoi(line.substr(line.find_first_of(digits, pos)));;
				pos = line.find("h");
				h = std::stoi(line.substr(line.find_first_of(digits, pos)));

				tgs::Rect rect(x, y, w, h);
				m_vrec.push_back(rect);
			}
			// load the texture
			m_texture = Texture_2d(texture_file);
		}
	}
	else {
		std::cerr << __FUNCTION__ << ": could not open file " << file_name << std::endl;
	}

}

//get the uv coordinates for the sprite number 'sprite_no' in the texture atlas
void tgs::Sprite_atlas::get_text_coord(const int sprite_no, tgs::Rect *prect, float * px_width, float * px_height) const
{
	
//	cgm::vec2 uv0, uv1, uv2, uv3;
	const tgs::Rect & rect = m_vrec[sprite_no];

//	uv0.x = rect.x / m_image_size.x;
//	uv0.y = rect.y / m_image_size.y;

//	uv1.x = uv0.x;
//	uv1.y = uv0.y + rect.height / m_image_size.y;

//	uv2.x = uv0.x + rect.width / m_image_size.x;
//	uv2.y = uv1.y;

//	uv3.x = uv2.x;
//	uv3.y = uv0.y;
	prect->x       = rect.x / m_image_size.x;
	prect->y       =  rect.y / m_image_size.y;
	prect->width   =  rect.width / m_image_size.x;
	prect->height  =  rect.height / m_image_size.y;
	
	*px_width  = rect.width;
	*px_height = rect.height;

	//return Rect(rect.x / m_image_size.x, rect.y / m_image_size.y, rect.width / m_image_size.x, rect.height / m_image_size.y);

	/*
	m_vertices_uv.clear();
	m_vertices_uv.push_back(uv1);
	m_vertices_uv.push_back(uv2);
	m_vertices_uv.push_back(uv3);

	m_vertices_uv.push_back(uv3);
	m_vertices_uv.push_back(uv0);
	m_vertices_uv.push_back(uv1);
	*/
}