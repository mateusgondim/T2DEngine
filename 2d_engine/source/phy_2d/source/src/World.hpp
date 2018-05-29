#ifndef _WORLD_HPP

#define _WORLD_HPP

#include "Body_2d.hpp"
#include "vec2.hpp"
#include <vector>
#include "Pool_allocator.hpp"

/*World: Part of the physics engine, this class represents a 
 * world populated with 2d bodies, this bodies are affected
 * by the  physics simulation and.
 *
 */

//TODO: COPY CONTROLL

#define MAX_BODY_2DS 40

class Tile_map;
namespace math { struct Rect; }

namespace physics_2d { class Collision_listener; struct Body_2d_def; }

#define MIN(a, b) ((a < b) ?(a) :(b))
#define MAX(a, b) ((a > b) ?(a) :(b))
//#define FLOAT_ROUNDOFF 0.001f
//#define AABB_COLL_OFFSET 0.05f
namespace physics_2d {
	class World final {
	public:
		enum Solver_methods { DISPLACE, ELASTIC };
		
		explicit World(const math::vec2 & gravity, const math::vec2 & solid_tile_sensor_line = math::vec2(3.2f, 3.5f) );
		World(const World &) = delete;
		//World(World &&) implement
		//World & operator=(World &&) implement
		World & operator=(const World &) = delete;
		~World();

		Body_2d *create_body_2d(const Body_2d::Entity_types & type, const math::vec2 & pos, const float m, const AABB_2d & aabb);
		Body_2d *create_body_2d(const Body_2d_def & body_def);
		void     destroy_body_2d(Body_2d *pbody);
		void     set_collision_listener(Collision_listener * pcoll_listener) { m_pcoll_listener =pcoll_listener; } //parameter should be poiter to interface that implements collision listener
		void     set_tile_map(const Tile_map * pmap) { m_pmap = pmap; }
		void     update(const float dt);
		bool     is_body_2d_on_ground(const Body_2d * pbody) const;
		bool     try_climbing_ladder(Body_2d * pbody, const bool is_climbing_up);
		bool     is_body_on_ladder(Body_2d *   pbody);
		bool     is_on_ladder_top_tile(const Body_2d * pbody, math::Rect & ladder_bounds) const;
	private:
		void check_n_solve_map_collision(Body_2d *pbody, const float dt) ;
		
		math::vec2										 m_solid_tile_sensor_line; // ofsset in pixels, against solid tiles for collision detection  
		math::vec2										 m_gravity;
		const Tile_map									*m_pmap;
		Collision_listener								*m_pcoll_listener;
		
		std::vector<Body_2d*>        m_bodies;
		mem::Pool_allocator			 m_mem_pool;
	};
}


#endif // !_WORLD_HPP
