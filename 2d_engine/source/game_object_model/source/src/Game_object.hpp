#ifndef _GAME_OBJECT_HPP
#define _GAME_OBJECT_HPP

#include "vec3.hpp"
#include "mat4.hpp" //chage to mat3!!!
#include "Transform.hpp"
#include "Sprite.hpp"
#include "Animator_controller.hpp"
#include "Body_2d.hpp"
#include "string_id.hpp"
#include <string>

/* Game_object: The base class for all the entities in the game world. 
 * This engine uses a component approach to its object model,
 * so the game object class works as a hub of components, each providing a specific functionallity
 * to the object, classes that inherent from this class, should initialize the needed components and,
 * add new ones if necessary.
 */
//TODO: CHANGE THE ORIENTATION TO BE A 3X3 MATRIX!
//TODO: MANAGE COPY CONTROLL!!!!
class Game_object {
public:
	Game_object() : m_psprite(nullptr), m_panimator_controller(nullptr), m_pbody_2d(nullptr) {}
	Game_object(const math::Transform & transform) : m_transform(transform), m_psprite(nullptr), m_panimator_controller(nullptr), m_pbody_2d(nullptr) {}
	Game_object(const math::vec3 & position) : m_transform(position), m_psprite(nullptr), m_panimator_controller(nullptr), m_pbody_2d(nullptr) {}
	//MISSING COPY CONTROLL!!!!!!!!

	virtual ~Game_object() 
	{
		delete m_psprite;
		delete m_panimator_controller;
		//delete m_pbody_2d; // need to delete through the physic_2d's world object 
	}

	math::Transform     & get_transform_component()		  { return m_transform; }
	gfx::Sprite          *get_sprite_component()		  { return m_psprite; }
	gfx::Animator_controller  *get_anim_controller_component() { return m_panimator_controller; }
	physics_2d::Body_2d  *get_body_2d_component()		  { return m_pbody_2d;}
	
	virtual std::string get_type() const = 0;
	virtual void update() = 0;
protected:
	math::Transform				 m_transform;//
	gfx::Sprite					*m_psprite;// 4 bytes
	gfx::Animator_controller	*m_panimator_controller; // 4 bytes
	physics_2d::Body_2d			*m_pbody_2d; // 4 bytes
	string_id					m_type;     // 4 bytes
	//vtable pointer + 4 bytes
	
	//TODO: PERHAPS ADD A MAP COLLISION FUNCTION!!
};

#endif // !_GAME_OBJECT_HPP
