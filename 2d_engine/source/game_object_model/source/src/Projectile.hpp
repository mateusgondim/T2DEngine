#ifndef _PROJECTILE_HPP
#define _PROJECTILE_HPP
#include "Game_object.hpp"
#include <utility>

namespace gfx { class Sprite_atlas; class Animator_controller; }
namespace physics_2d { struct Body_2d_def; }
namespace math { struct vec2; struct vec3; }

namespace gom {
	class Projectile : public Game_object {
	public:
		typedef std::pair <const gfx::Sprite_atlas*, uint8_t> atlas_n_layer;
		Projectile(const game_object_id unique_id, const uint16_t handle_index, const math::vec3 & pos, atlas_n_layer & sprite_data, physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller);
		//Projectile(const game_object_id unique_id, const uint16_t handle_index, atlas_n_layer & sprite_data, physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller);
		
		void set_direction(const math::vec2 & dir);
		void respawn(const math::vec3 & pos, const math::vec2 & direction);
		void update(const float dt) override;
	private:
		float m_damage;
		float m_speed;
		bool  m_is_facing_right = false; // REMOVE THIS!!!!!!!! WE SHOULD USE ROTATION MATRICES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
};
}
#endif // !_PROJECTILE_HPP