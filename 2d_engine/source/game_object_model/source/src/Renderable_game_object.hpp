#ifndef _RENDERABLE_GAME_OBJECT_HPP
#define _RENDERABLE_GAME_OBJECT_HPP


#include <string>
#include "vec3.hpp"
#include "mat4.hpp"
#include "Sprite.hpp"
#include "Game_object.hpp"


/*Renderable_game_object : Abstract Base Class, it represents a game object in the world
 * that has a visual representation in the form of a sprite
 */

class Renderable_game_object : public Game_object {
public:
	Renderable_game_object(const cgm::vec3 & position, const cgm::mat4 & orientation, const std::string & file_name, const float pixels_per_unit = 16);
	Renderable_game_object(const std::string & file_name, const float pixels_per_unit = 16);
	virtual void update() = 0;
	tgs::Sprite & get_sprite();
private:
	tgs::Sprite	m_sprite;

};

#endif // !_RENDERABLE_GAME_OBJECT_HPP
