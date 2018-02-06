//
//  Shader.cpp
//  
//
//  Created by Mateus_Gondim on 15/06/16.
//
//
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "Shader.hpp"

using std::string;
using std::ifstream;
using std::cerr;
using std::endl;

gls::Shader::Shader(const string & vert_file_name, const string & frag_file_name, const string & geom_file_name )
{
    ifstream in(vert_file_name);
    
    string line;
    string source;
    while (getline(in, line)) {
        source += line + '\n';
    }
    in.close();
    
    GLchar *vert_src = (GLchar *) source.c_str();
    
    GLuint vert_obj = glCreateShader(GL_VERTEX_SHADER);
    
    glShaderSource(vert_obj, 1, &vert_src, NULL);
    glCompileShader(vert_obj);
    
    GLint is_compiled = 0;
    glGetShaderiv(vert_obj, GL_COMPILE_STATUS, &is_compiled);
    
    if (is_compiled == GL_FALSE) {
        GLint max_length = 0;
        glGetShaderiv(vert_obj, GL_INFO_LOG_LENGTH, &max_length);
        
		std::vector<GLchar> error_log(max_length);
        glGetShaderInfoLog(vert_obj, max_length, &max_length, &error_log[0]);
        
		std::string message = std::string(error_log.begin(), error_log.end());

        cerr << "Error compiling vertex shader\n Log: \n" << message << endl;
        
        glDeleteShader(vert_obj);
    }
    
    source.clear();
    in.open(frag_file_name);
    while (getline(in, line)) {
        source += line + '\n';
    }
    in.close();
    
    GLchar *frag_src = (GLchar *) source.c_str();
    
    GLuint frag_obj = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(frag_obj, 1, &frag_src, NULL);
    glCompileShader(frag_obj);
    
    glGetShaderiv(frag_obj, GL_COMPILE_STATUS, &is_compiled);
    
    if (is_compiled == GL_FALSE) {
        GLint max_length = 0;
        glGetShaderiv(frag_obj, GL_INFO_LOG_LENGTH, &max_length);
        
		std::vector<GLchar> error_log(max_length);
        glGetShaderInfoLog(frag_obj, max_length, &max_length , &error_log[0]);
        
		std::string message = std::string(error_log.begin(), error_log.end());

		cerr << "Error compiling fragment shader\n Log: \n" << message << endl;
        
        glDeleteShader(frag_obj);
    }
	
	//geometry shader
	bool include_geom_shader = false;
	GLuint geom_obj;
	if (!geom_file_name.empty()) {
		source.clear();
		in.open(geom_file_name);
		while (getline(in, line)) {
			source += line + '\n';
		}
		in.close();

		GLchar *geom_src = (GLchar *)source.c_str();
	    geom_obj = glCreateShader(GL_GEOMETRY_SHADER);

		glShaderSource(geom_obj, 1, &geom_src, NULL);
		glCompileShader(geom_obj);

		glGetShaderiv(geom_obj, GL_COMPILE_STATUS, &is_compiled);
		include_geom_shader = true;
		if (is_compiled == GL_FALSE) {
			GLint max_length = 0;
			glGetShaderiv(geom_obj, GL_INFO_LOG_LENGTH, &max_length);

			std::vector<GLchar> error_log(max_length);
			glGetShaderInfoLog(geom_obj, max_length, &max_length, &error_log[0]);

			std::string message = std::string(error_log.begin(), error_log.end());

			cerr << "Error compiling Geometry shader\n Log: \n" << message << endl;

			include_geom_shader = false;
			glDeleteShader(geom_obj);
		}
	}
	
    m_program = glCreateProgram();
    
    glAttachShader(m_program, vert_obj);
    glAttachShader(m_program, frag_obj);
	if (include_geom_shader) {
		glAttachShader(m_program, geom_obj);
	}

    glLinkProgram(m_program);
    
    glDetachShader(m_program, vert_obj);
    glDetachShader(m_program, frag_obj);
	
	if (include_geom_shader) {
		glDetachShader(m_program, geom_obj);
		glDeleteShader(geom_obj);
	}
    glDeleteShader(vert_obj);
    glDeleteShader(frag_obj);
}

gls::Shader::Shader(const std::string & compute_file) 
{
	ifstream in(compute_file);

	string line;
	string source;
	while (getline(in, line)) {
		source += line + '\n';
	}
	in.close();

	GLchar *compute_src = (GLchar *)source.c_str();

	GLuint compute_obj = glCreateShader(GL_COMPUTE_SHADER);

	glShaderSource(compute_obj, 1, &compute_src, NULL);
	glCompileShader(compute_obj);

	GLint is_compiled = 0;
	
	glGetShaderiv(compute_obj, GL_COMPILE_STATUS, &is_compiled);
	if (is_compiled == GL_FALSE) {
		GLint max_length = 0;
		glGetShaderiv(compute_obj, GL_INFO_LOG_LENGTH, &max_length);

		std::vector<GLchar> error_log(max_length);
		glGetShaderInfoLog(compute_obj, max_length, &max_length, &error_log[0]);

		std::string message = std::string(error_log.begin(), error_log.end());

		cerr << "Error compiling compute shader\n Log: \n" << message << endl;

		glDeleteShader(compute_obj);
	}

	m_program = glCreateProgram();
	glAttachShader(m_program, compute_obj);

	glLinkProgram(m_program);

	glDetachShader(m_program, compute_obj);
	glDeleteShader(compute_obj);
}

GLint gls::Shader::get_uniform_location(const std::string & u_name) const
{
	GLint location = glGetUniformLocation(m_program, u_name.c_str());

	if (location == -1) {
		std::cerr << "ERROR: Could not find uniform '" << u_name << "' in shader program" << std::endl;
	}

	return location;
}