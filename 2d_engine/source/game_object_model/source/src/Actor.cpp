#include "Actor.hpp"

#include "vec3.hpp"
#include "vec2.hpp"
#include "mat4.hpp"
#include "Body_2d_def.hpp"
#include "Body_2d.hpp"
#include "World.hpp"
#include "Physics_manager.hpp"

#include "Sprite.hpp"
#include "Sprite_atlas.hpp"
#include "Graphics_manager.hpp"
#include "Gameplay_state.hpp"
#include "Animator_controller.hpp"

#include "runtime_memory_allocator.hpp"





//TAKE OUT MAGIC NUMBER 16 ON SPRITE CONSTRUCTOR
//Actor::Actor(const cgm::vec3 & pos, const cgm::mat4 & orientation, const std::string & texture_file, State *pstate, const AABB_2d & aabb, const cgm::vec2 & velocity, bool facing_left)
//	: Renderable_game_object(pos, orientation, texture_file), m_pstate(pstate), m_aabb(aabb), m_velocity(velocity), m_facing_left(facing_left) {}
namespace gom {

	Actor::Actor(const game_object_id unique_id, const uint16_t handle_index, atlas_n_layer & sprite_data, physics_2d::Body_2d_def *pbody_def, const gfx::Animator_controller *pcontroller, bool facing_left)
		: Game_object(unique_id, handle_index, pbody_def->m_position), m_pstate(nullptr), m_velocity(pbody_def->m_velocity), m_facing_left(facing_left)
	{
		//create sprite component
		void *pmem = mem::allocate(sizeof(gfx::Sprite));
		if (pmem != nullptr) {
			m_psprite = static_cast<gfx::Sprite*>(new (pmem) gfx::Sprite(sprite_data.first, sprite_data.second) );
			
			//add to the graphics manager to be rendered
			gfx::g_graphics_mgr.add_sprite_to_render(m_psprite);
		}

		//create a body2d component
		m_pbody_2d = physics_2d::g_physics_mgr.get_world()->create_body_2d(*pbody_def);
		m_pbody_2d->set_user_data(static_cast<void*>(this));

		//make a copy of the animator controller
		pmem = mem::allocate(sizeof(gfx::Animator_controller));
		if (pmem != nullptr) {
			m_panimator_controller = static_cast<gfx::Animator_controller*>( new (pmem) gfx::Animator_controller(*pcontroller) );
		}
	}

	Actor::~Actor() 
	{
		if (m_pstate != nullptr) {
			size_t sz = m_pstate->get_size();
			m_pstate->~Gameplay_state();
			mem::free(m_pstate, sz);
			m_pstate = nullptr;
		}
	}

	void Actor::begin_tile_collision(const physics_2d::AABB_2d & tile_aabb)
	{
		m_pstate->begin_tile_collision(*this, tile_aabb);
	}
	void Actor::end_tile_collision(const physics_2d::AABB_2d & tile_aabb)
	{
		m_pstate->end_tile_collision(*this, tile_aabb);
	}
}