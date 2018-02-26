//
//  Shader.h
//  
//
//  Created by Mateus_Gondim on 15/06/16.
//
//

#ifndef _SHADER_HPP

#define _SHADER_HPP

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <string>
namespace gfx {
	class Shader {
	public:
		Shader(const std::string & vert_file_name, const std::string & frag_file_name, const std::string & geom_file_name = "");
		Shader(const std::string & compute_file);
		void     use()        const { glUseProgram(m_program); }
		GLuint  get_program() const { return m_program; }
		GLint   get_uniform_location(const std::string & u_name) const;
		//void    bind_attrib(const std::string & name, )
	private:
		GLuint m_program;
		//std::map<std::string GLuint> m_attribs;
	};
}
#endif