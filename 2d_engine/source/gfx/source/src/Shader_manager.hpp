#ifndef _SHADER_MANAGER_HPP
#define _SHADER_MANAGER_HPP	

#include "Resource_manager.hpp"

namespace rms { class Resource; }

namespace gfx {
	class Shader_manager : public rms::Resource_manager {
	public:
		Shader_manager();
		rms::Resource *load(const char *name, const char *vert_fname, const char *frag_fname, const char *geom_fname = nullptr);

		rms::Resource *create_impl(const char *name) override;
	};
}

#endif // !_SHADER_MANAGER_HPP
