#ifndef _TEXTURE_2D_MANAGER_HPP
#define _TEXTURE_2D_MANAGER_HPP

#include "Resource_manager.hpp"

namespace rms { class Resource; }


namespace gfx {
	class Texture_2d_manager : public rms::Resource_manager {
	public:
		Texture_2d_manager();

		rms::Resource *load(const char *name, const char* texture_file_path);
		rms::Resource *create_impl(const char *name) override;
	};
}
#endif // !_TEXTURE_2D_MANAGER_HPP
