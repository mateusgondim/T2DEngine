#ifndef _CAMERA_2D_HPP
#define _CAMERA_2D_HPP

#include "vec3.hpp"
#include "vec2.hpp"
#include "mat4.hpp"
#include "Transform.hpp"

namespace gfx {
	class Camera_2d {
	public:
		Camera_2d() = default;
		Camera_2d(float tile_width_wld, float tile_height_wld, float tiles_per_screen_width, float tiles_per_screen_height, int map_width, int map_height ,const math::vec2 & map_wld_origin = math::vec2() );

		void init(float tile_width_wld, float tile_height_wld, float tiles_per_screen_width, float tiles_per_screen_height, int map_width, int map_height, const math::vec2 & map_wld_origin = math::vec2());
		math::vec3  &  get_position() { return m_transform.get_translation(); }
		math::mat4  &  projection()   { return m_projection; }
		void           translate(const math::vec3 & t) { m_transform.get_translation() += t; }
		void		   follow(const math::vec3 & p);
		bool		   is_off_camera(const math::vec3 & pos, const float width, const float height) const;
		math::mat4   & get_view() const { return math::mat4(math::mat4(), -m_transform.get_translation()); }

		void           set_screen_dim(const float tiles_per_screen_width, const float tiles_per_screen_height);
		void		   scale(const float device_aspect_ratio);
	private:
		math::Transform  m_transform;
		math::mat4		 m_projection;			//orthographic projection matrix
		float			 m_tile_width_wld;			// in world units
		float			 m_tile_height_wld;		    // in world units
		float			 m_tiles_per_screen_width;  // the width of the screen in tiles 
		float			 m_tiles_per_screen_height; // the height of the screen in tiles
		float			 m_aspect_ratio;
		float			 m_left;
		float			 m_right;
		float			 m_bottom;
		float			 m_top;

		math::vec2		 m_screen_p_max;
		math::vec2       m_screen_p_min;
		math::vec2		 m_map_wld_p_max;
		math::vec2		 m_map_wld_p_min;

		//maybe a pointer to the actor it is tracking...

	};
}
#endif // !_CAMERA_2D_HPP
