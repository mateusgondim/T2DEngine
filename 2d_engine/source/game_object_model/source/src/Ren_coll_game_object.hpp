#ifndef _REN_COLL_GAME_OBJECT_HPP
#define _REN_COLL_GAME_OBJECT_HPP


#include <string>
#include "vec3.hpp"
#include "mat4.hpp"
#include "Sprite.hpp"
#include "Game_object.hpp"
#include "Body_2d.hpp"

class Ren_coll_game_object : public Game_object {
public:
	//Ren_coll_game_object(const cgm::vec3 & position, const cgm::mat4 & orientation, const std::string & file_name, const float pixels_per_unit = 16) : 
	//	Game_object(position, orientation), m_sprite(file_name, pixels_per_unit), m_pbody(nullptr) {}
	
	Ren_coll_game_object(const std::string & file_name, const float pixels_per_unit = 16) : m_sprite(file_name, pixels_per_unit) {}
	
	virtual void update() = 0;
	virtual std::string get_type() const = 0;
	gfx::Sprite &		  get_sprite()  { return m_sprite; }
	physics_2d::Body_2d * get_body_2d() { return m_pbody; }
	
	void set_body_2d(physics_2d::Body_2d * pbody) 
	{
		if (m_pbody == nullptr) {
			m_pbody = pbody;
		}
	}
private:
	gfx::Sprite	         m_sprite;
	physics_2d::Body_2d  *m_pbody;
};

#endif // !_REN_COLL_GAME_OBJECT_HPP
