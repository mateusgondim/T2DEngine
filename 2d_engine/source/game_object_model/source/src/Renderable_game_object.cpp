#include <string>
#include "Renderable_game_object.hpp"
#include "vec3.hpp"
#include "mat4.hpp"
#include "Game_object.hpp"
#include "Sprite.hpp"

Renderable_game_object::Renderable_game_object(const cgm::vec3 & position, const cgm::mat4 & orientation, const std::string & file_name, const float pixels_per_unit ) :
	Game_object(position, orientation), m_sprite(file_name, pixels_per_unit) {}

Renderable_game_object::Renderable_game_object(const std::string & file_name, const float pixels_per_unit ) : m_sprite(file_name, pixels_per_unit) {}

tgs::Sprite & Renderable_game_object::get_sprite() { return m_sprite; }