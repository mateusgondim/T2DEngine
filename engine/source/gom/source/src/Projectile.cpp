#include "Projectile.hpp"
#include "Game_object.hpp"
#include "Camera_2d.hpp"

#include "vec2.hpp"
#include "vec3.hpp"
#include "crc32.hpp"
#include "Variant.hpp"
#include "Event_arguments.hpp"
#include "Event.hpp"

#include "Body_2d_def.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"

#include "Sprite.hpp"
#include "Animator_controller.hpp"
#include "Graphics_manager.hpp"

#include "Game_object_manager.hpp"
#include "runtime_memory_allocator.hpp"

#include <iostream>

namespace gom {
	Projectile::Projectile(const game_object_id unique_id, const uint16_t handle_index,
                           const math::vec3 & pos, atlas_n_layer & sprite_data,
                           physics_2d::Body_2d_def *pbody_def,
                           const gfx::Animator_controller *pcontroller) :
		gom::Game_object(unique_id, handle_index, pos), m_hit(false), m_damage(20)
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
		m_speed = math::vec2_mag(m_pbody_2d->get_velocity());

		//make a copy of the animator controller
		pmem = mem::allocate(sizeof(gfx::Animator_controller));
		if (pmem != nullptr) {
			m_panimator_controller = static_cast<gfx::Animator_controller*>(new (pmem) gfx::Animator_controller(*pcontroller));
		}
	}

    void Projectile::on_event(Event & event)
    {
            switch (event.get_type()) {
            case SID('EVENT_BEGIN_COLLISION'): {
                    if (m_hit) {
                            break;
                    }

                    const Variant *phandle_arg = event.get_arguments()
                            .find(SID('game_object_handle_index'));

                    const Variant *pobject_id_arg = event.get_arguments()
                            .find(SID('game_object_id'));

                    gom::Game_object_handle handle_to_target(pobject_id_arg->m_as_string_id,
                            phandle_arg->m_as_uint16);

                    Game_object * pgame_object = gom::g_game_object_mgr.get_by_handle(handle_to_target);
                    if (!pgame_object) {
                            std::cout << __FUNCTION__ << " ERROR: Could not find game object"
                                    << std::endl;
                            break;
                    }

                    // create Projectile Attack Event
                    Event projectile_attack(SID('EVENT_PROJECTILE_ATTACK'));
                    projectile_attack.get_arguments().insert(SID('game_object_id'),
                            get_unique_id());

                    projectile_attack.get_arguments().insert(SID('game_object_handle_index'),
                            get_handle_index());

                    projectile_attack.get_arguments().insert(SID('attack_points'), get_damage());

                    // send event
                    pgame_object->on_event(projectile_attack);
                    break;
            }
            case SID('EVENT_END_COLLISION'):
                    break;
            case SID('EVENT_ATTACK'):
                    break;
            case SID('EVENT_HIT_TARGET'):
                    m_hit = true;
                    break;
            }
    }

	void Projectile::update(const float dt) 
	{
            Camera_2d *plevel_camera = g_game_object_mgr.get_main_camera();
		if (plevel_camera->is_off_camera(m_pbody_2d->get_position(), 1.0F, 1.0F)) {
			set_active(false);
			return;
		}
		
		if (m_hit) {
			set_active(false);
			return;
		}
		else {
			m_panimator_controller->update(dt);

			gfx::Animator_state & curr_state = m_panimator_controller->get_current_state();
			if (curr_state.changed_animation_frame()) {
				m_psprite->update_uv(curr_state.get_curr_anim_frame());
			}
			m_psprite->update_pos(m_pbody_2d->get_position(), m_is_facing_right);
		}
	}

	void Projectile::set_direction(const math::vec2 & dir) 
	{
		math::vec2 vel = m_speed * dir;
		m_pbody_2d->set_velocity(vel);
		m_is_facing_right = (dir.x >= 0.0f) ? (true) : (false);
	}

	void Projectile::respawn(const math::vec3 & pos, const math::vec2 & direction)
	{
		m_transform.get_translation() = pos;
		set_direction(direction);
		m_pbody_2d->set_position(math::vec2(pos.x, pos.y));
		m_hit = false;
		set_active(true);
	}
}