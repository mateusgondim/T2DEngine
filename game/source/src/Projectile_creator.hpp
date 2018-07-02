#ifndef _PROJECTILE_CREATOR_HPP
#define _PROJECTILE_CREATOR_HPP	

#include "Creator.hpp"
#include "string_id.hpp"

namespace gom  { class Game_object; }
namespace math { struct vec3; }
namespace gfx { class Animator_controller; }
namespace physics_2d { struct Body_2d_def; }

class Projectile_creator : public gom::Creator {
public:
	Projectile_creator(const string_id atlas_id, const physics_2d::Body_2d_def & body_def, const gfx::Animator_controller *panim_controller);
	gom::Game_object *create(void * pmem, const uint32_t unique_id, const uint16_t handle_index, const math::vec3 & wld_pos) override;

	~Projectile_creator() override;
private:
	string_id				 m_atlas_res_id;
	//string_id                m_anim_controller_id;
	gfx::Animator_controller *m_pcontroller;
};

#endif // !_PROJECTILE_CREATOR_HPP
