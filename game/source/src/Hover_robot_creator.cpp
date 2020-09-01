#include "Hover_robot_creator.hpp"
#include "Game_object.hpp"
#include "Creator.hpp"
#include "Actor.hpp"
#include "Hover_robot.hpp"

#include "string_id.hpp"

#include "vec2.hpp"

#include <iostream>

#include "crc32.hpp"
#include "Sprite_atlas.hpp"
#include "Animator_controller.hpp"
#include "Animation_player.hpp"
#include "Animator_state.hpp"

#include "Body_2d_def.hpp"
#include "Body_2d.hpp"
#include "Collider_2d_def.hpp"
#include "AABB_2d.hpp"
#include "Object.hpp"

#include "runtime_memory_allocator.hpp"

Hover_robot_creator::Hover_robot_creator(const string_id atlas_res_id,
                                         const string_id anim_controll_id) : 
        gom::Creator(), m_atlas_res_id(atlas_res_id), m_anim_controller_id(anim_controll_id)
{
	void *pmem = mem::allocate(sizeof(physics_2d::Body_2d_def));
	if (pmem) {
		m_pbody_def = static_cast<physics_2d::Body_2d_def*>(new (pmem) physics_2d::Body_2d_def());
		math::vec2 pos(8.0f, 35.0f);

		physics_2d::AABB_2d p_aabb(math::vec2(-0.32f, -0.44f), math::vec2(0.32f, 0.44f));
		p_aabb.p_max += pos;
		p_aabb.p_min += pos;

		m_pbody_def->m_type = physics_2d::Body_2d::Entity_types::DYNAMIC;
		m_pbody_def->m_mass = 1.0f;
		m_pbody_def->m_gravity_scale = 1.0f;
		m_pbody_def->m_position = pos;
		m_pbody_def->m_linear_velocity = math::vec2(0.0f, 0.0f);
		m_pbody_def->m_vel_threshold = math::vec2(6.0f, 12.0f);
		m_pbody_def->m_aabb = p_aabb;
	}
	else {
		std::cerr << "ERROR(" << __FUNCTION__ << "): Unable to allocate memory for Body_2d_def" 
                  << std::endl;
	}

	create_anim_controller();
}

void Hover_robot_creator::create_anim_controller() 
{
	void *pmem = mem::allocate(sizeof(gfx::Animator_controller));
	if (!pmem) {
		//DEBUG HERE
		std::cerr << "ERROR(" << __FILE__ << "): Unable to allocate memory for controller" << std::endl;
		return;
	}

	m_panim_controller = static_cast<gfx::Animator_controller*>(new (pmem) gfx::Animator_controller);
	
    gfx::Animation idle_anim({ SID('robot_idle01'), SID('robot_idle02'),
                               SID('robot_idle03'), SID('robot_idle04') }, 4);
            
	gfx::Animation_player idle_player(idle_anim);

	gfx::Animator_state & idle_state = m_panim_controller->add_state("hover_robot_idle",
                                                                     idle_player);
}

Hover_robot_creator::~Hover_robot_creator() 
{
	mem::free(static_cast<void*>(m_panim_controller), sizeof(gfx::Animator_controller));
}


gom::Game_object * Hover_robot_creator::create(const Object & obj_description) 
{
	// get the data for creating the sprite componenent
	gfx::Sprite_atlas *patlas = static_cast<gfx::Sprite_atlas*>(
        gfx::g_sprite_atlas_mgr.get_by_id(m_atlas_res_id));

	gom::Actor::atlas_n_layer sprite_data(patlas, 1);

	math::vec2	tr = math::vec2(obj_description.get_x() - m_pbody_def->m_position.x,
                                obj_description.get_y() - m_pbody_def->m_position.y);

	m_pbody_def->m_position += tr;
	m_pbody_def->m_aabb.p_max += tr;
	m_pbody_def->m_aabb.p_min += tr;

	physics_2d::Collider_2d_def coll_def;
	coll_def.m_aabb = m_pbody_def->m_aabb;
	coll_def.m_is_trigger = true;

    std::size_t object_sz = sizeof(Hover_robot);
    void *pmem = mem::allocate(object_sz);
	gom::Game_object *pgame_object = static_cast<gom::Game_object*>(
        new (pmem) Hover_robot(object_sz, sprite_data, m_pbody_def, m_panim_controller));

	pgame_object->get_body_2d_component()->create_collider_2d(coll_def);
	pgame_object->set_type(m_obj_type);
    pgame_object->set_tag(m_obj_tag);

	return pgame_object;
}