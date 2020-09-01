#include "Projectile_creator.hpp"
#include "Creator.hpp"
#include "Game_object.hpp"
#include "Projectile.hpp"

#include "vec2.hpp"
#include "vec3.hpp"
#include "AABB_2d.hpp"

#include "Animator_controller.hpp"
#include "Animator_state.hpp"
#include "Sprite_atlas.hpp"
#include "Sprite_atlas_manager.hpp"

#include "string_id.hpp"

#include "Collider_2d_def.hpp"
#include "Body_2d_def.hpp"

#include "runtime_memory_allocator.hpp"
#include "Object.hpp"

#include <utility>

Projectile_creator::Projectile_creator(const string_id atlas_id,
                                       const physics_2d::Body_2d_def & body_def,
                                       const gfx::Animator_controller *panim_controller) : 
        gom::Creator(), m_atlas_res_id(atlas_id)
{
	//create body_2d_def
	void *pmem = mem::allocate(sizeof(physics_2d::Body_2d_def));
	if (pmem != nullptr) {
		m_pbody_def = static_cast<physics_2d::Body_2d_def*>( new (pmem) physics_2d::Body_2d_def(body_def));
	}
	
	//makea copy of this anim controller
	pmem = mem::allocate(sizeof(gfx::Animator_controller));
	m_pcontroller = static_cast<gfx::Animator_controller*>(new (pmem) gfx::Animator_controller(*panim_controller));
}

gom::Game_object *Projectile_creator::create(const Object & obj_description) 
{
	//get the data to create sprite component
	gfx::Sprite_atlas *patlas = static_cast<gfx::Sprite_atlas*>(gfx::g_sprite_atlas_mgr.get_by_id(m_atlas_res_id));
	gom::Projectile::atlas_n_layer data(patlas, 1);

    math::vec3 wld_pos(obj_description.get_x(), obj_description.get_y());
	math::vec2 translation = math::vec2(wld_pos.x, wld_pos.y) - m_pbody_def->m_position;

	m_pbody_def->m_position   += translation;
	m_pbody_def->m_aabb.p_max += translation;
	m_pbody_def->m_aabb.p_min += translation;

	physics_2d::Collider_2d_def coll_def;
	coll_def.m_aabb = m_pbody_def->m_aabb;

    std::size_t object_sz = sizeof(gom::Projectile);
    void *pmem = mem::allocate(object_sz);
    gom::Game_object *pgame_object = static_cast<gom::Game_object*>(
        new (pmem) gom::Projectile(object_sz, wld_pos, data, m_pbody_def, m_pcontroller));

	pgame_object->get_body_2d_component()->create_collider_2d(coll_def);
	pgame_object->set_type(m_obj_type);
    pgame_object->set_tag(m_obj_tag);

	return pgame_object;
}

Projectile_creator::~Projectile_creator() 
{
	mem::free(static_cast<void*>(m_pcontroller), sizeof(gfx::Animator_controller));
}