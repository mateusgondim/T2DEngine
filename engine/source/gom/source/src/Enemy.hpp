#ifndef _ENEMY_HPP
#define _ENEMY_HPP
#include "Actor.hpp"

namespace gfx { class Sprite_atlas; }
namespace physics_2d { struct AABB_2d; class Body_2d; struct Body_2d_def; }
namespace gom { class Projectile; }
namespace gom {
	class Enemy : public Actor { 
	public:
		Enemy(const game_object_id unique_id, const uint16_t handle_index, atlas_n_layer & sprite_data, physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller, bool facing_left, const int heath, const int damage);
		virtual void projectile_collision(Projectile *pprojectile);
		virtual void update(const float dt) = 0;
		int get_health() const;
		int get_damage() const;
	protected:
		int m_health;
		int m_damage;
	};

	inline int Enemy::get_health() const 
	{
		return m_health;
	}

	inline int Enemy::get_damage() const 
	{
		return m_damage;
	}
}

#endif // !_ENEMY_HPP
