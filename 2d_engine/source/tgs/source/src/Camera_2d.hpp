#ifndef _CAMERA_2D_HPP
#define _CAMERA_2D_HPP

#include "vec3.hpp"
#include "mat4.hpp"
#include "Transform.hpp"

namespace tgs {
	class Camera_2d {
	public:
		Camera_2d(float tile_width, float tile_height, float screen_tile_width, float screen_tile_height);

		cgm::vec3  &  get_position() { return m_transform.get_translation(); }
		cgm::mat4  &  projection()   { return m_projection; }
		void          translate(const cgm::vec3 & t) { m_transform.get_translation() += t; }
		void follow(const cgm::vec3 & p);
		cgm::mat4  get_view() const { return cgm::mat4(cgm::mat4(), -m_transform.get_translation()); }
	private:
		cgm::Transform m_transform;
		cgm::mat4 m_projection;			//orthographic projection matrix
		float     m_tile_width;			// in world units
		float     m_tile_height;		// in world units
		float     m_screen_tile_width;  // the width of the screen in tiles 
		float     m_screen_tile_height; // the height of the screen in tiles
		//maybe a pointer to the actor it is tracking...

	};
}
#endif // !_CAMERA_2D_HPP
