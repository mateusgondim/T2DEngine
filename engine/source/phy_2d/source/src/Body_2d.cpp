#include "Body_2d.hpp"
#include "Body_2d_def.hpp"
#include "Collider_2d_def.hpp"
#include "Collider_2d.hpp"
#include "World.hpp"

namespace physics_2d {
	//Body_2d::Body_2d(const Entity_types & type, const math::vec2 & pos, const float m, const AABB_2d & aabb)
	//	: m_type(type), m_position(pos), m_mass(m), m_aabb(aabb), m_vel_threshold(math::vec2(0.0f, 0.0f)) {}

	Body_2d::Body_2d(const Body_2d_def *pbody_def, World *pworld) :
		m_type(pbody_def->m_type), m_position(pbody_def->m_position), m_velocity(pbody_def->m_linear_velocity), m_vel_threshold(pbody_def->m_vel_threshold),  m_mass(pbody_def->m_mass), m_acceleration(pbody_def->m_acceleration),
	    m_gravity_scale(pbody_def->m_gravity_scale), m_map_collision(pbody_def->m_map_collision), m_pworld(pworld), m_pprev(nullptr), m_pnext(nullptr), m_pcollider(nullptr) {}

	Collider_2d *Body_2d::create_collider_2d(const Collider_2d_def & coll_def) 
	{
		// allocate memory for collider
		void *pmem = m_pworld->m_collider_allocator.get_element();
		if (pmem) {
			//create collider
			Collider_2d *pcoll = static_cast<Collider_2d*>( new (pmem) Collider_2d);
			pcoll->Create(this, &coll_def, m_pworld->m_collider_proxy_allocator);
			m_pcollider = pcoll;

			// add a collider proxy inside the world::m_pbroadphase
			m_pcollider->Create_proxy(*m_pworld->m_pbroadphase);
		}
		return m_pcollider;
	}

	void Body_2d::set_active(const bool is_active) 
	{
		if (is_active == m_is_active) {
			return;
		}

		m_is_active = is_active;
		if (is_active) {
			//create proxy
			m_pcollider->Create_proxy(*m_pworld->m_pbroadphase);
		}
		else {
			//remove contacts with proxy in the world
			m_pworld->remove_contacts_with_proxy(m_pcollider->m_pproxy);

			//destroy proxy
			m_pcollider->destroy_proxy(*m_pworld->m_pbroadphase);
		}
	}

	Body_2d::~Body_2d() 
	{
		destroy_collider_2d();
	}

	void Body_2d::destroy_collider_2d() 
	{
		m_pcollider->destroy_proxy(*m_pworld->m_pbroadphase);
		m_pcollider->Destroy(m_pworld->m_collider_proxy_allocator);
		m_pworld->m_collider_allocator.free_element(static_cast<void*>(m_pcollider));
	}
	
}