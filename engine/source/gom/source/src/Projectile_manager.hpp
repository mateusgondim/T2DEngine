#ifndef _PROJECTILE_MANAGER_HPP
#define _PROJECTILE_MANAGER_HPP	

#include "Projectile.hpp"
#include <vector>
#include <cstdint>

namespace math { struct vec2; struct vec3; }
namespace gom { class Game_object_handle; }

namespace gom {
	class Projectile_manager {
	public:
		typedef uint32_t type_id;
		Projectile_manager() = default;
		
		void init();
		void shut_down();
		void update(const float dt);
		Game_object_handle spawn_projectile(const type_id id, const math::vec3 & pos, const math::vec2 & projectile_dir);

		~Projectile_manager() {}
	private:
		std::vector<Projectile*> m_vpactive_projectiles;
		std::vector<Projectile*> m_vpinactive_projectiles;
	};
	extern Projectile_manager g_projectile_mgr;
}
#endif // !_PROJECTILE_MANAGER_HPP
