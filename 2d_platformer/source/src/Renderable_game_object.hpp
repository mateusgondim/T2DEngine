#ifndef _RENDERABLE_GAME_OBJECT_HPP
#define _RENDERABLE_GAME_OBJECT_HPP

#include <memory>
#include <string>
#include "Game_object.hpp"
#include "Sprite.hpp"

/*Renderable_game_object : Abstract Base Class, it represents a game object in the world
 * that has a visual representation in the form of a sprite
 */

class Renderable_game_object : public Game_object {
public:
	Renderable_game_object(const cgm::vec3 & position, const cgm::mat4 & orientation, const std::string & file_name, const float pixels_per_unit = 16) :
		Game_object(position, orientation), m_sprite(file_name, pixels_per_unit) {}
	
	Renderable_game_object(const std::string & file_name,  const float pixels_per_unit = 16) : m_sprite(file_name, pixels_per_unit){}
	virtual void update(const float delta_time) = 0;
	 tgs::Sprite & get_sprite()  { return m_sprite; }
private:
	tgs::Sprite	m_sprite;

};

#endif // !_RENDERABLE_GAME_OBJECT_HPP
