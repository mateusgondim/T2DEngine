#ifndef _SPRITE_HPP
#define _SPRITE_HPP

#include "string_id.hpp"
#include "vec2.hpp"
#include "vec3.hpp"
#include <stdint.h>

/*
 *
 */
//TODO: Maybe add a change direction function
namespace gfx { class Sprite_atlas; }

#define POS_ARRAY_SZ 6
#define UV_ARRAY_SZ  6

namespace gfx {
	class Sprite {
	public:
		Sprite(const Sprite_atlas *patlas, const uint8_t layer);
		const	math::vec3 * get_vertex_position_vec()  const  { return m_vertices_pos; }
				math::vec3 * get_vertex_position_vec()         { return m_vertices_pos;}
		const	math::vec2 * get_vertex_uv_vec()		const  { return m_vertices_uv; };
				math::vec2 * get_vertex_uv_vec()	           { return m_vertices_uv; };
				bool		 is_active()			    const  { return m_is_active; }
	    const   Sprite_atlas *get_atlas()		        const;
				uint8_t      get_layer()				const  { return m_layer;}

				void         set_active(const bool flag)       { m_is_active = flag;}
		void   update_pos(const math::vec3 & position, const bool facing_left = true);
		void   update_uv(const string_id sprite_id);
		//bool   is_animated() const;
		//void   update_animation(const float dt, Animator_controller *pcontroller);

		~Sprite() = default;

	private:
		math::vec3							 m_vertices_pos[POS_ARRAY_SZ]; // size = 72 bytes | alignment = 4 bytes
		math::vec2							 m_vertices_uv[UV_ARRAY_SZ];	// size = 48 bytes | alignment = 4 bytes
		
		bool								 m_is_active;
        math::vec2                           m_size_in_wld_units;
		const Sprite_atlas					*m_patlas;	//MAYBE CHANGE TO AN ID
		uint8_t								 m_layer;
		
		//TODO: SCALE THE VERTICES IN THE POSITION VECTOR WHEN CHANGING THE CURRENT VERTEX UV COORDINATES
		//std::vector<unsigned> m_elements
	};
}

#endif // !_SPRITE_HPP
