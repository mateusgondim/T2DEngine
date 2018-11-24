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

#include "Resource.hpp"

namespace rms { class Resource_manager; }

namespace gfx {
	class Shader : public rms::Resource {
	public:
		Shader(rms::Resource_manager *pcreator, const char *name);
		//hader(const std::string & vert_file_name, const std::string & frag_file_name, const std::string & geom_file_name = "");
		//Shader(const std::string & compute_file);

		void	load() override;
		void	unload() override;
		size_t  calculate_size() const override;

		void   set_vert_file_name(const std::string & vert_fname);
		void   set_frag_file_name(const std::string & frag_fname);
		void   set_geom_file_name(const std::string & geom_fname);
		void     use()        const;
		std::uint32_t  get_program() const { return m_program; }
		std::int32_t   get_uniform_location(const std::string & u_name) const;
        void uniform_matrix4fv(std::int32_t location, std::int32_t count, bool is_transposed, const float *pvalue);
        void uniform_1f(std::int32_t location, float v0);
		//void    bind_attrib(const std::string & name, )
	private:
		std::uint32_t m_program;
		std::string   m_vert_fname, m_frag_fname, m_geom_fname;
	};
}
#endif