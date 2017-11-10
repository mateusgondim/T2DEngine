#ifndef _GAME_OBJECT_HPP
#define _GAME_OBJECT_HPP

#include "vec3.hpp"
#include "mat4.hpp" //chage to mat3!!!
#include <string>

/*Game_object: The base class for all the entities
 *  in the game world, it represents a object that exists in
 *  the game world, updates itself and can clone itself using the prototype pattern
 */
//TODO: CHANGE THE ORIENTATION TO BE A 3X3 MATRIX!
class Game_object {
public:
	Game_object() = default;
	Game_object(const cgm::vec3 & position, const cgm::mat4 & orientation) : m_position(position), m_orientation(orientation) {}
	
	virtual ~Game_object() = default;
	cgm::vec3 &	get_position() { return m_position; }
	cgm::mat4 & object_to_upright() { return m_orientation; }
	
	virtual std::string get_type() const = 0;
	virtual void update() = 0;
private:
	cgm::vec3						m_position;
	cgm::mat4						m_orientation;
	
	/*
	const tgs::Sprite & get_sprite() const { return *m_psprite; }
	std::shared_ptr<tgs::Sprite>	m_psprite;
	Box_collider_2d					m_box_col;
	cgm::vec2						m_velocity;
	float							m_anim_frame;
	float							m_life;
	*/
	//TODO: PERHAPS ADD A MAP COLLISION FUNCTION!!
};

#endif // !_GAME_OBJECT_HPP
