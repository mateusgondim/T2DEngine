#include "Projectile_manager.hpp"

#include "vec3.hpp"

#include "Game_object_handle.hpp"
#include "Game_object_manager.hpp"

#include <iostream>

namespace gom {
	Projectile_manager g_projectile_mgr;

	void Projectile_manager::init() 
	{
		m_vpactive_projectiles.reserve(50);
		m_vpinactive_projectiles.reserve(50);
	}

	void Projectile_manager::shut_down() 
	{
		m_vpactive_projectiles.clear();
		m_vpinactive_projectiles.clear();
	}

	Game_object_handle Projectile_manager::spawn_projectile(const type_id id, const math::vec3 & pos, const math::vec2 & projectile_dir) 
	{
		if (m_vpinactive_projectiles.empty()) {
			Game_object_handle handle = g_game_object_mgr.instantiate(id, pos);

			Projectile * pprojectile = static_cast<Projectile*>(g_game_object_mgr.get_by_handle(handle));
			pprojectile->set_direction(projectile_dir);
			m_vpactive_projectiles.push_back(pprojectile);

			return handle;
		}
		else {//SEARCH BY TYPE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			Projectile *pprojectile = m_vpinactive_projectiles.back();
			pprojectile->respawn(pos, projectile_dir);
			
			m_vpinactive_projectiles.pop_back();
			m_vpactive_projectiles.push_back(pprojectile);
			
			return Game_object_handle(*pprojectile);
		}
	}

	void Projectile_manager::update(const float dt) 
	{
		std::vector<Projectile*>::iterator it = m_vpactive_projectiles.begin();
		while (it != m_vpactive_projectiles.end()) {
			if ( !(*it)->is_active()) {
				//projectile was deactivated
				m_vpinactive_projectiles.push_back(*it);
				it = m_vpactive_projectiles.erase(it);
			}
			else {
				++it;
			}
		}
	}
}