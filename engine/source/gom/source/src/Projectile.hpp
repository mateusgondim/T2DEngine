#ifndef _PROJECTILE_HPP
#define _PROJECTILE_HPP
#include "Game_object.hpp"
#include <utility>
#include <cstddef>

namespace gfx { class Sprite_atlas; class Animator_controller; }
namespace physics_2d { struct Body_2d_def; }
namespace math { struct vec2; struct vec3; }
class Event;

namespace gom {
	class Projectile : public Game_object {
	public:
		typedef std::pair <const gfx::Sprite_atlas*, uint8_t> atlas_n_layer;
		Projectile(std::size_t object_sz, const math::vec3 & pos, atlas_n_layer & sprite_data,
                   physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller);

		void set_direction(const math::vec2 & dir);
		void set_hit(const bool is_hit);
		bool get_hit() const;
		int  get_damage() const;
		void respawn(const math::vec3 & pos, const math::vec2 & direction);
		void update(const float dt) override;
        void on_event(Event & event) override;
	private:
		int   m_damage;
		float m_speed;
		bool  m_hit; // 
		bool  m_is_facing_right = false; // REMOVE THIS!!!!!!!! WE SHOULD USE ROTATION MATRICES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	};
	
	inline int Projectile::get_damage() const 
	{
		return m_damage;
	}

	inline void Projectile::set_hit(const bool is_hit) 
	{
		m_hit = is_hit;
	}
	
	inline bool Projectile::get_hit() const 
	{
		return m_hit;
	}
}
#endif // !_PROJECTILE_HPP
