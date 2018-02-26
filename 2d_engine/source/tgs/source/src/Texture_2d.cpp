#include "Texture_2d.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "SOIL.h"

#include <string>
#include <iostream>

tgs::Texture_2d::Texture_2d(const std::string & image_path) : m_id(0), m_pref_count(nullptr)
{
	load_texture(image_path);
}

tgs::Texture_2d::Texture_2d(const Texture_2d & t) : m_pref_count(t.m_pref_count), m_id(t.m_id), m_width(t.m_width), m_height(t.m_height), m_image_path(t.m_image_path)
{
	++*m_pref_count; // increment reference count
}

void tgs::Texture_2d::load_texture(const std::string & image_path)
{
	// ******DEBUG *****
#ifndef NDEBUG
	std::cout << __FILE__ << " Creating a texture object. Using image data from file: " << image_path << std::endl;
#endif // !NDEBUG
	glGetError();
	if (glIsTexture(m_id) == GL_FALSE) {
		m_image_path = image_path;

		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		int width, height;
		unsigned char* image = SOIL_load_image(m_image_path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

		m_width = width;
		m_height = height;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		//there is one use of this texture id
		m_pref_count = new int(1);

		if (GL_NO_ERROR != glGetError()) {
			std::cerr << __FUNCTION__ << " failed to create texture" << std::endl;
		}
	}
	else {
		std::cerr << __FUNCTION__ << " failed to create texture: " << m_id << " is already a texture object name, call delete_texture() to delete the currently bound texture" << std::endl;
	}
}

tgs::Texture_2d & tgs::Texture_2d::operator=(const Texture_2d & rhs) 
{
	++*rhs.m_pref_count; // increment right hand side reference count
	if (--*m_pref_count == 0) {
		delete m_pref_count;
		delete_texture();
	}

	m_pref_count  =  rhs.m_pref_count;
	m_id		  =  rhs.m_id;
	m_width		  =  rhs.m_width;
	m_height	  =  rhs.m_height;
	m_image_path  =  rhs.m_image_path;

	return *this;
}

tgs::Texture_2d::~Texture_2d() 
{
	if (--*m_pref_count == 0) {
		delete m_pref_count;
		delete_texture();
	}

}

void tgs::Texture_2d::use(const unsigned unit) const
{
	if (glIsTexture(m_id) == GL_TRUE) {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}
	else {
		std::cerr << __FUNCTION__ << " failed to use texture: " << m_id << " is not a name of a texture, use load_texture(image_path) to load a 2d texture" << std::endl;
	}
}



void tgs::Texture_2d::delete_texture() 
{
	if (glIsTexture(m_id) == GL_TRUE) {
		glDeleteTextures(1, &m_id);
		m_id = 0;
		if (GL_NO_ERROR != glGetError()) {
			std::cerr << __FUNCTION__ << " failed to delete texture" << std::endl;
		}
	}
	else if (m_id != 0) {
		std::cerr << __FUNCTION__ << " failed to delete texture: " << m_id << " is not a name of a texture" << std::endl;
	}
}