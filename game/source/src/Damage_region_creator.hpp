#ifndef _DAMAGE_REGION_CREATOR_HPP
#define _DAMAGE_REGION_CREATOR_HPP

#include "Creator.hpp"

namespace gom { class Game_object; }
namespace math { struct vec3; }
class Object;

class Damage_region_creator : public gom::Creator {
public:
    Damage_region_creator();

    gom::Game_object * create(const Object & obj_description) override;
};
#endif