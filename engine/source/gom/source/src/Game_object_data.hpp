#ifndef _GAME_OBJECT_DATA_HPP
#define _GAME_OBJECT_DATA_HPP

#include "string_id.hpp"
#include "vec3.hpp"

namespace gom {
	struct Game_object_data {
		string_id	object_type_id;
		math::vec3	position;
	};
}
#endif // !_GAME_OBJECT_DATA_HPP
