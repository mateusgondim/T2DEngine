#ifndef _GAME_OBJECT_HPP
#define _GAME_OBJECT_HPP

#include "Transform.hpp"
#include <stdint.h>

/* Game_object: The base class for all the entities in the game world. 
 * This engine uses a component approach to its object model,
 * so the game object class works as a hub of components, each providing a specific functionallity
 * to the object, classes that inherent from this class, should initialize the needed components and
 * add new ones if necessary.
 */
//TODO: CHANGE THE ORIENTATION TO BE A 3X3 MATRIX!
//TODO: MANAGE COPY CONTROLL!!!!

namespace gfx { class Sprite; class Animator_controller; }
namespace physics_2d { class Body_2d; }
namespace gom {

	class Game_object {
	public:
		typedef uint32_t game_object_id;

		Game_object(const game_object_id unique_id, const uint16_t handle_index);
		Game_object(const game_object_id unique_id, const uint16_t handle_index, const math::Transform & transform);
		Game_object(const game_object_id unique_id, const uint16_t handle_index, const math::vec3 & position);
		//MISSING COPY CONTROLL!!!!!!!!
		virtual ~Game_object();

		game_object_id        get_unique_id() const;
		uint16_t			  get_handle_index() const;
		bool				  is_active() const { return m_is_active; }
		void				  set_active(const bool flag);
		
		void				 set_tag(const uint32_t object_tag) { m_tag = object_tag; }
		uint32_t			 get_tag() const { return m_tag; }

		math::Transform     & get_transform_component();
		gfx::Sprite          *get_sprite_component();
		gfx::Animator_controller  *get_anim_controller_component();
		physics_2d::Body_2d  *get_body_2d_component();

		virtual void update(const float dt) = 0;
	protected:
		math::Transform				 m_transform;//
		gfx::Sprite					*m_psprite;// 4 bytes
		gfx::Animator_controller	*m_panimator_controller; // 4 bytes
		physics_2d::Body_2d			*m_pbody_2d; // 4 bytes
	private:
		game_object_id	m_unique_id;
		uint16_t		m_handle_index;
		
		uint32_t		m_tag; //tag used to identify the game object's type, as "player" or "enemy"

		bool			m_is_active;


		//TODO: PERHAPS ADD A MAP COLLISION FUNCTION!!
	};
}
#endif // !_GAME_OBJECT_HPP
