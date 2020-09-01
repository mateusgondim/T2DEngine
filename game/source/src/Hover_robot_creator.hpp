#ifndef _HOVER_ROBOT_CREATOR_HPP
#define _HOVER_ROBOT_CREATOR_HPP

#include "Creator.hpp"
#include "string_id.hpp"
#include <stdint.h>

namespace math { struct vec3; }
namespace gom { class Game_object; }
namespace gfx { class Animator_controller; }
class Object;

class Hover_robot_creator : public gom::Creator {
public:
	Hover_robot_creator(const string_id atlas_res_id, const string_id anim_controll_id);
	gom::Game_object * create(const Object & obj_description) override;

	~Hover_robot_creator() override;
private:
	void create_anim_controller();

	gfx::Animator_controller  *m_panim_controller;
	string_id  m_atlas_res_id;
	string_id  m_anim_controller_id;
};

#endif // !_HOVER_ROBOT_CREATOR_HPP
