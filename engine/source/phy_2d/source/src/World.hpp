#ifndef _WORLD_HPP

#define _WORLD_HPP

#include "Body_2d.hpp"
#include "vec2.hpp"
#include <vector>
#include <cstddef>
#include "Pool_allocator.hpp"
#include "Tile.hpp"

/*World: Part of the physics engine, this class represents a 
 * world populated with 2d bodies, this bodies are affected
 * by the  physics simulation and.
 *
 */

//TODO: COPY CONTROLL

#define MAX_BODY_2DS 40
#define MAX_CONTACTS 80

class Tile_map;
namespace math { struct Rect; struct ray3; }

namespace physics_2d { class Collision_listener; struct Body_2d_def; struct Collider_2d_proxy ; class Broadphase; }

namespace physics_2d {
	
	//contact data, used to store pointers to pairs of Collider_2ds, that are currently colliding
	struct Contact_node {
		Collider_2d_proxy *pproxy_a;
		Collider_2d_proxy *pproxy_b;
		Contact_node *pnext;
		Contact_node *pprev;
	};
	
	class World final {
		friend class Body_2d;
	public:
		enum Solver_methods { DISPLACE, ELASTIC };
		
		explicit World(const math::vec2 & gravity, const math::vec2 & solid_tile_sensor_line = math::vec2(3.0f, 3.4f) );
		World(const World &) = delete;
		//World(World &&) implement
		//World & operator=(World &&) implement
		World & operator=(const World &) = delete;
		~World();

		Body_2d *get_body_list();
		int32_t  get_body_count() const;

		//Body_2d *create_body_2d(const Body_2d::Entity_types & type, const math::vec2 & pos, const float m, const AABB_2d & aabb);
		Body_2d *create_body_2d(const Body_2d_def & body_def);
		void     destroy_body_2d(Body_2d *pbody);
		void     set_collision_listener(Collision_listener * pcoll_listener) { m_pcoll_listener =pcoll_listener; } //parameter should be poiter to interface that implements collision listener
		void	 set_broadphase(Broadphase *pbroadphase);
		void     set_tile_map(const Tile_map * pmap) { m_pmap = pmap; }
		void     update(const float dt);
		bool     is_body_2d_on_ground(const Body_2d * pbody) const;
		bool     try_climbing_ladder(Body_2d * pbody, const bool is_climbing_up);
		bool     is_body_on_ladder(Body_2d *   pbody);
		bool     is_on_ladder_top_tile(const Body_2d * pbody, math::Rect & ladder_bounds) const;
        bool     trace_on_map(const math::ray3 & r, const Tile **phit, const unsigned steps) const;
	private:
		void add_to_contact_list(Collider_2d_proxy *pprox_a, Collider_2d_proxy *pproxy_b);
		void remove_from_contact_list(physics_2d::Contact_node *pcontact);
		void remove_contacts_with_proxy(Collider_2d_proxy *pcoll_proxy);
		void check_n_solve_map_collision(Body_2d *pbody, const float dt) ;
		
		math::vec2										 m_solid_tile_sensor_line; // ofsset in pixels, against solid tiles for collision detection  
		math::vec2										 m_gravity;
		const Tile_map									*m_pmap;
		Collision_listener								*m_pcoll_listener;
		Broadphase										*m_pbroadphase;
		
		Body_2d				*m_pbody_list;
		int32_t				 m_body_cnt;

		//list of currently colliding bodies
		Contact_node *m_pcontact_list;
		size_t		  m_contact_cnt;

		std::vector<Body_2d*>        m_bodies;
		
		//Allocators
		mem::Pool_allocator			 m_body_allocator;
		mem::Pool_allocator          m_collider_allocator;
		mem::Pool_allocator          m_collider_proxy_allocator;
		mem::Pool_allocator		     m_contact_allocator;
	};

	inline void World::set_broadphase(Broadphase * pbroadphase) 
	{
		m_pbroadphase = pbroadphase;
	}

	inline Body_2d *World::get_body_list() 
	{
		return m_pbody_list;
	}

	inline int32_t World::get_body_count() const 
	{
		return m_body_cnt;
	}
}


#endif // !_WORLD_HPP
