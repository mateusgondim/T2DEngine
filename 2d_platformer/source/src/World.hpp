#ifndef _WORLD_HPP

#define _WORLD_HPP

#include "Body_2d.hpp"
#include "Tile_map.hpp"
#include "vec2.hpp"
#include <vector>

/*World: Part of the physics engine, this class represents a 
 * world populated with 2d bodies, this bodies are affected
 * by the  physics simulation and.
 *
 */


namespace physics_2d {
	class World final {
	public:
		enum Entity_types { KINEMATIC, DYNAMIC };
		enum Solver_methods { DISPLACE, ELASTIC };
		
		explicit World(const cgm::vec2 & gravity);
		Body_2d *create_body_2d(const Entity_types & type, const cgm::vec2 pos, const cgm::vec2 & vel, const float m, const AABB_2d & aabb);
		void     destroy_body_2d(Body_2d *);
		void     set_collision_listener(); //parameter should be poiter to intetace that implements collision listener
		void     set_tile_map(const Tile_map * pmap);
		update();

		~World();
	private:
		void check_n_solve_map_collision(Body_2d *pbody) const;
		const Tile_map  *m_pmap;
		//pointer to tile_map here...
		//pointer to collision listener
		cgm::vec2			   m_gravity;
		std::vector<Body_2d*>  m_bodies;
	};
}


#endif // !_WORLD_HPP
