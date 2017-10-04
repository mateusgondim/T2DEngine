#ifndef _SPRITE_HPP
#define _SPRITE_HPP

#include "vec2.hpp"
#include "vec3.hpp"
#include "Sprite_atlas.hpp"
#include "Animator_controller.hpp"
#include <vector>
#include <memory>
#include <string>

/*
 *
 */
//TODO: Maybe add a change direction function
namespace tgs {
	class Sprite {
	public:
		Sprite(const std::string & file_name, const float pixels_per_unit = 16.0f);
		const	std::vector<cgm::vec3> & get_vertex_position_vec() const { return m_vertices_pos; }
				std::vector<cgm::vec3> & get_vertex_position_vec()       { return m_vertices_pos;}
		const	std::vector<cgm::vec2> & get_vertex_uv_vec()	   const { return m_vertices_uv; };
				unsigned                 get_pixels_per_unit()	   const {return m_pixels_per_unit;}
		std::shared_ptr<const Sprite_atlas> get_atlas()		       const {return m_atlas;}

		void       set_anim_controller(std::unique_ptr<Animator_controller> & uptr_anim_controller);
		Animator_controller *get_panim_controller() { return m_upanim_controller.get(); }

		void   update_pos(const cgm::vec3 & position, const bool facing_left = true);
		void   update_uv(const int sprite_no);
		bool   is_animated() const;
		void   update_animation();
	private:
		std::vector<cgm::vec3>				m_vertices_pos;
		std::vector<cgm::vec2>				m_vertices_uv;
		float								m_pixels_per_unit; // number of pixels per game world's unit
		cgm::vec2                           m_scale;           // scale taking into account the pixels per world unit
		std::shared_ptr<const Sprite_atlas>	m_atlas;
		std::unique_ptr<Animator_controller>  m_upanim_controller;
		//TODO: SCALE THE VERTICES IN THE POSITION VECTOR WHEN CHANGING THE CURRENT VERTEX UV COORDINATES
		//std::vector<unsigned> m_elements
	};
}

#endif // !_SPRITE_HPP
