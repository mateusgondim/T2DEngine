#ifndef _WORLD_HPP

#define _WORLD_HPP

#include "Body_2d.hpp"
#include "Tile_contact.hpp"
#include "Collision_listener.hpp"
#include "Tile_map.hpp"
#include "vec2.hpp"
#include <vector>
#include <set>

/*World: Part of the physics engine, this class represents a 
 * world populated with 2d bodies, this bodies are affected
 * by the  physics simulation and.
 *
 */

//TODO: COPY CONTROLL

#define MIN(a, b) ((a < b) ?(a) :(b))
#define FLOAT_ROUNDOFF 0.001f
#define AABB_COLL_OFFSET 0.1f
namespace physics_2d {
	class World final {
	public:
		enum Solver_methods { DISPLACE, ELASTIC };
		
		explicit World(const cgm::vec2 & gravity);
		World(const World &) = delete;
		//World(World &&) implement
		//World & operator=(World &&) implement
		World & operator=(const World &) = delete;
		~World();

		Body_2d *create_body_2d(const Body_2d::Entity_types & type, const cgm::vec2 & pos, const float m, const AABB_2d & aabb);
		void     destroy_body_2d(Body_2d *pbody);
		void     set_collision_listener(Collision_listener * pcoll_listener) { m_pcoll_listener =pcoll_listener; } //parameter should be poiter to interface that implements collision listener
		void     set_tile_map(const Tile_map * pmap) { m_pmap = pmap; }
		void     update();
		bool     is_body_2d_on_ground(const Body_2d * pbody) const;
	private:
		static bool compare(const Tile_contact & lhs, const Tile_contact & rhs) { return lhs.m_pbody < rhs.m_pbody; }
		void check_n_solve_map_collision(Body_2d *pbody) ;
		
		cgm::vec2										 m_gravity;
		const Tile_map									*m_pmap;
		Collision_listener								*m_pcoll_listener;
		std::multiset<Tile_contact, decltype(compare)*>  m_tile_contact_multiset{compare};
		std::vector<Body_2d*>        m_bodies;
	};
}


#endif // !_WORLD_HPP
