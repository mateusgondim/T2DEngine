#include "Texture_2d.hpp"
#include "Resource.hpp"
#include "Resource_manager.hpp"
#include "string_id.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//#include "SOIL.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <cstdint>

gfx::Texture_2d::Texture_2d(rms::Resource_manager *pcreator, const char *name) : rms::Resource(pcreator, name), m_texture_id(0), m_image_path(nullptr) {}

void gfx::Texture_2d::set_image_path(const char *file_path) 
{
	if (m_image_path != nullptr) {
		free(m_image_path);
	}
	m_image_path = strdup(file_path);
}

void gfx::Texture_2d::load() 
{
#ifndef NDEBUG
	std::cout << __FILE__ << " Creating a texture object. Using image data from file: " << m_image_path << std::endl;
#endif // !NDEBUG
	glGetError();
	if (glIsTexture(m_texture_id) == GL_FALSE) {
		
		glGenTextures(1, &m_texture_id);
		glBindTexture(GL_TEXTURE_2D, m_texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		std::int32_t  width, height, num_channels;
		
		//std::uint8_t  *image = SOIL_load_image(m_image_path, &width, &height, 0, SOIL_LOAD_RGBA);
		std::uint8_t	*image = stbi_load(m_image_path, &width, &height, &num_channels, 4);

		m_width = width;
		m_height = height;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		glGenerateMipmap(GL_TEXTURE_2D);
		
		//SOIL_free_image_data(image);
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (GL_NO_ERROR != glGetError()) {
			std::cerr << __FUNCTION__ << " failed to create texture" << std::endl;
		}
	}
	else {
		std::cerr << __FUNCTION__ << " failed to create texture: " << m_texture_id << " is already a texture object name, call delete_texture() to delete the currently bound texture" << std::endl;
	}
}

void gfx::Texture_2d::unload() 
{
	remove_string_id_entry(m_id);
	free(m_image_path);

	if (glIsTexture(m_texture_id) == GL_TRUE) {
		glDeleteTextures(1, &m_texture_id);
		m_texture_id = 0;

		if (GL_NO_ERROR != glGetError()) {
			std::cerr << __FUNCTION__ << " failed to delete texture" << std::endl;
		}
	}
	else if (m_texture_id != 0) {
		std::cerr << __FUNCTION__ << " failed to delete texture: " << m_texture_id << " is not a name of a texture" << std::endl;
	}
}

size_t gfx::Texture_2d::calculate_size() const
{
	return m_width * m_height * GL_UNSIGNED_BYTE;
}


/*gfx::Texture_2d::Texture_2d(const std::string & image_path) : m_texture_id(0), m_pref_count(nullptr)
{
	load_texture(image_path);
}*/

/*
gfx::Texture_2d::Texture_2d(const Texture_2d & t) : 
	m_pref_count(t.m_pref_count), m_texture_id(t.m_texture_id), m_width(t.m_width), m_height(t.m_height), m_file_name_id(t.m_file_name_id)
{
	++*m_pref_count; // increment reference count
}*/

/*
void gfx::Texture_2d::load_texture(const std::string & image_path)
{
	// ******DEBUG *****
#ifndef NDEBUG
	std::cout << __FILE__ << " Creating a texture object. Using image data from file: " << image_path << std::endl;
#endif // !NDEBUG
	glGetError();
	if (glIsTexture(m_texture_id) == GL_FALSE) {
		m_file_name_id = intern_string(image_path.c_str()); //generate the string_id for this texture, usint it's file_path 

		glGenTextures(1, &m_texture_id);
		glBindTexture(GL_TEXTURE_2D, m_texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		std::int32_t  width, height;
		std::uint8_t  *image = SOIL_load_image(image_path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

		m_width = width;
		m_height = height;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

	
		m_pref_count = new int(1);

		if (GL_NO_ERROR != glGetError()) {
			std::cerr << __FUNCTION__ << " failed to create texture" << std::endl;
		}
	}
	else {
		std::cerr << __FUNCTION__ << " failed to create texture: " << m_texture_id << " is already a texture object name, call delete_texture() to delete the currently bound texture" << std::endl;
	}
}*/

/*
gfx::Texture_2d & gfx::Texture_2d::operator=(const Texture_2d & rhs)
{
	++*rhs.m_pref_count; // increment right hand side reference count
	if (--*m_pref_count == 0) {
		delete m_pref_count;
		delete_texture();
	}

	m_pref_count    =  rhs.m_pref_count;
	m_texture_id    =  rhs.m_texture_id;
	m_width		    =  rhs.m_width;
	m_height	    =  rhs.m_height;
	m_file_name_id  =  rhs.m_file_name_id;

	return *this;
}

gfx::Texture_2d::~Texture_2d()
{
	if (--*m_pref_count == 0) {
		delete m_pref_count;
		delete_texture();
	}

}*/

void gfx::Texture_2d::use(const std::uint32_t unit) const
{
	if (glIsTexture(m_texture_id) == GL_TRUE) {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_texture_id);
	}
	else {
		std::cerr << __FUNCTION__ << " failed to use texture: " << m_texture_id << " is not a name of a texture, use load_texture(image_path) to load a 2d texture" << std::endl;
	}
}


/*
void gfx::Texture_2d::delete_texture()
{
	if (glIsTexture(m_texture_id) == GL_TRUE) {
		glDeleteTextures(1, &m_texture_id);
		m_texture_id = 0;
		remove_string_id_entry(m_file_name_id);

		if (GL_NO_ERROR != glGetError()) {
			std::cerr << __FUNCTION__ << " failed to delete texture" << std::endl;
		}
	}
	else if (m_texture_id != 0) {
		std::cerr << __FUNCTION__ << " failed to delete texture: " << m_texture_id << " is not a name of a texture" << std::endl;
	}
}*/