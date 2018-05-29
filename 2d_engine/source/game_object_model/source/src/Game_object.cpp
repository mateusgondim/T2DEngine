#include "Game_object.hpp"
#include "Transform.hpp"
#include "Sprite.hpp"
#include "Graphics_manager.hpp"
#include "Body_2d.hpp"
#include "Animator_controller.hpp"

#include "runtime_memory_allocator.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"

namespace gom {
	Game_object::Game_object(const game_object_id unique_id, const uint16_t handle_index) :
		m_unique_id(unique_id), m_handle_index(handle_index), m_psprite(nullptr), m_panimator_controller(nullptr), m_pbody_2d(nullptr) {}

	Game_object::Game_object(const game_object_id unique_id, const uint16_t handle_index, const math::Transform & transform) 
		: m_unique_id(unique_id), m_handle_index(handle_index), m_transform(transform), m_psprite(nullptr), m_panimator_controller(nullptr), m_pbody_2d(nullptr) {}

	Game_object::Game_object(const game_object_id unique_id, const uint16_t handle_index, const math::vec3 & position)
		: m_unique_id(unique_id), m_handle_index(handle_index), m_transform(position), m_psprite(nullptr), m_panimator_controller(nullptr), m_pbody_2d(nullptr) {}

	Game_object::~Game_object()
	{
		if (m_psprite != nullptr) {
			//remove it from the graphics manager
			gfx::g_graphics_mgr.rem_sprite_from_batch(m_psprite);
			
			//destroy and deallocate 
			m_psprite->~Sprite();
			mem::free(static_cast<void*>(m_psprite), sizeof(gfx::Sprite));

			m_psprite = nullptr;
		}
		if (m_panimator_controller != nullptr) {
			m_panimator_controller->~Animator_controller();
			mem::free(static_cast<void*>(m_panimator_controller), sizeof(gfx::Animator_controller));

			m_panimator_controller = nullptr;
		}
		if (m_pbody_2d != nullptr) {
			physics_2d::g_physics_mgr.get_world()->destroy_body_2d(m_pbody_2d);
			m_pbody_2d = nullptr;
		}
	}

	Game_object::game_object_id Game_object::get_unique_id() const
	{
		return m_unique_id;
	}

	uint16_t Game_object::get_handle_index() const
	{
		return m_handle_index;
	}

	math::Transform & Game_object::get_transform_component()
	{
		return m_transform;
	}
	gfx::Sprite *Game_object::get_sprite_component()
	{
		return m_psprite;
	}

	gfx::Animator_controller *Game_object::get_anim_controller_component()
	{
		return m_panimator_controller;
	}

	physics_2d::Body_2d  *Game_object::get_body_2d_component()
	{
		return m_pbody_2d;
	}
}