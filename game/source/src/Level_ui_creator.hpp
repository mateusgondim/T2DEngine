#ifndef _LEVEL_UI_CREATOR_HPP
#define _LEVEL_UI_CREATOR_HPP

#include "Creator.hpp"
#include "string_id.hpp"

namespace gom  { class Game_object; }
namespace math { struct vec3; }
class Object;

class Level_ui_creator final : public gom::Creator {
public:
        Level_ui_creator(const string_id canvas_atlas_id);
        gom::Game_object * create(const Object & obj_description) override;
private:
        string_id m_canvas_atlas_id;
};

#endif