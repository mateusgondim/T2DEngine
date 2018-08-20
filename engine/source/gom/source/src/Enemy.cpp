#include "Enemy.hpp"

#include "Projectile.hpp"

namespace gom {

	Enemy::Enemy(const game_object_id unique_id, const uint16_t handle_index, atlas_n_layer & sprite_data, physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller, bool facing_left, const int heath, const int damage)
		: Actor(unique_id, handle_index, sprite_data, pbody_def, pcontroller, facing_left), m_health(heath), m_damage(damage) {}

	void Enemy::projectile_collision(Projectile *pprojectile) 
	{
		//By default, destroy projectile and Enemy
		
		/* change to set animator to collision, something like pprojectile->get_anim_controller()->set_trigger("explosion"). Remove the set_active(false)
		 * leave it to the Projectile to set itself to false, AFTER  the collision animation is complete.
		 * Change to get the damage and subtract from current health
		 * if heath <= 0, destroy enemy
		 */
		m_health -= pprojectile->get_damage();
		pprojectile->set_hit(true);
		
	}
}