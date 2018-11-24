#ifndef _CAMERA_2D_HPP
#define _CAMERA_2D_HPP

#include "Game_object_handle.hpp"
#include "vec3.hpp"
#include "vec2.hpp"
#include "mat4.hpp"
#include "Transform.hpp"
#include <stdint.h>

namespace gom {
	class Camera_2d {
	public:
            Camera_2d() = default;
		Camera_2d(float tile_width_wld, float tile_height_wld, float tiles_per_screen_width, float tiles_per_screen_height, int map_width, int map_height ,const math::vec2 & map_wld_origin = math::vec2() );

		void init(float tile_width_wld, float tile_height_wld, float tiles_per_screen_width, float tiles_per_screen_height, int map_width, int map_height, const math::vec2 & map_wld_origin = math::vec2());
		math::vec3  &  get_position() { return m_transform.get_translation(); }
		math::mat4  &  projection()   { return m_projection; }
        void           translate(const math::vec3 & t);
		
        bool           track(const Game_object *pobject);
        bool           track(const uint32_t obj_type);
		bool		   is_off_camera(const math::vec3 & pos, const float width, const float height) const;
		const math::mat4  &  get_view() const;

        void update(const float dt);

		void           set_screen_dim(const float tiles_per_screen_width, const float tiles_per_screen_height);
		void		   scale(const float device_aspect_ratio);
	private:
            void update_view();
            void		   follow(const math::vec3 & p);
            math::Transform             m_transform;
		math::mat4		 m_projection;			//orthographic projection matrix
        math::mat4       m_view;
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

        //variables the manage the traking of a game object by the Camera
        Game_object_handle              m_htracking_obj;
        uint32_t                        m_tracking_obj_type_id;
        bool                            m_is_tracking = false;

	};

    inline const math::mat4 & Camera_2d::get_view() const
    {
            return m_view;
    }

    inline void Camera_2d::update_view()
    {
            math::vec3 pos = m_transform.get_translation();
            m_view.set_translation(-pos);
    }

    inline void Camera_2d::translate(const math::vec3 & t) 
    { 
            m_transform.get_translation() += t; 
            update_view();
    }
}
#endif // !_CAMERA_2D_HPP
