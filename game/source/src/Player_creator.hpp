#ifndef _PLAYER_CREATOR_HPP
#define _PLAYER_CREATOR_HPP

#include "Creator.hpp"
#include "string_id.hpp"
#include <stdint.h>

namespace gfx { class Animator_controller; class Sprite_atlas_manager; }
namespace physics_2d { class World; }
namespace gom { class Game_object; }
namespace math { struct vec3; }

class Player_creator : public gom::Creator {
public:
	Player_creator(const string_id atlas_id, const string_id anim_controller_id);
	gom::Game_object *create(const math::vec3 & wld_pos) override;

	~Player_creator() override;
private:
	void create_anim_controller();
	gfx::Animator_controller  *m_panim_controller;
	string_id  m_atlas_res_id;
	string_id  m_anim_controller_id;

};

#endif // !_PLAYER_CREATOR_HPP
