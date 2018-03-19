//
//  Shader.h
//  
//
//  Created by Mateus_Gondim on 15/06/16.
//
//

#ifndef _SHADER_HPP

#define _SHADER_HPP
#include <string>
#include <cstdint>

namespace gfx {
	class Shader {
	public:
		Shader(const std::string & vert_file_name, const std::string & frag_file_name, const std::string & geom_file_name = "");
		Shader(const std::string & compute_file);
		void     use()        const;
		std::uint32_t  get_program() const { return m_program; }
		std::int32_t   get_uniform_location(const std::string & u_name) const;
		//void    bind_attrib(const std::string & name, )
		~Shader();
	private:
		std::uint32_t m_program;
		//std::map<std::string GLuint> m_attribs;
	};
}
#endif