#include "Projectile.hpp"
#include "Game_object.hpp"

#include "vec2.hpp"
#include "vec3.hpp"

#include "Body_2d_def.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"

#include "Sprite.hpp"
#include "Animator_controller.hpp"
#include "Graphics_manager.hpp"

#include "runtime_memory_allocator.hpp"

#include <iostream>

namespace gom {
	Projectile::Projectile(const game_object_id unique_id, const uint16_t handle_index, const math::vec3 & pos, atlas_n_layer & sprite_data, physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller) :
		gom::Game_object(unique_id, handle_index, pos)
	{
		//create the sprite component
		void *pmem = mem::allocate(sizeof(gfx::Sprite));
		if (pmem != nullptr) {
			m_psprite = static_cast<gfx::Sprite*>( new (pmem) gfx::Sprite(sprite_data.first, sprite_data.second));

			//add to the graphics manager to be rendered
			gfx::g_graphics_mgr.add_sprite_to_render(m_psprite);
		}

		//create the body_2d component
		m_pbody_2d = physics_2d::g_physics_mgr.get_world()->create_body_2d(*pbody_def);
		m_pbody_2d->set_user_data(static_cast<void*>(this));

		//make a copy of the animator controller
		pmem = mem::allocate(sizeof(gfx::Animator_controller));
		if (pmem != nullptr) {
			m_panimator_controller = static_cast<gfx::Animator_controller*>(new (pmem) gfx::Animator_controller(*pcontroller));
		}
	}

	void Projectile::update(const float dt) 
	{
		if (gfx::g_graphics_mgr.get_camera().is_off_camera(m_pbody_2d->get_position(), 1.0F, 1.0F)) {
			std::cout << "I'AM OFF CAMERA!!!!!!!" << std::endl;
			return;
		}

		m_panimator_controller->update(dt);

		gfx::Animator_state & curr_state = m_panimator_controller->get_current_state();
		if (curr_state.changed_animation_frame()) {
			m_psprite->update_uv(curr_state.get_curr_anim_frame());
		}
		m_psprite->update_pos(m_pbody_2d->get_position());
	}

	void Projectile::respawn(const math::vec3 & pos) 
	{
		m_transform.get_translation() = pos;
		m_pbody_2d->set_position(math::vec2(pos.x, pos.y));
		set_active(true);
	}
}