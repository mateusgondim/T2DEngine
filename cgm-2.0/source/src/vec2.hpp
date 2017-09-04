#ifndef _VEC2_HPP

#define _VEC2_HPP
namespace cgm {
	struct vec2 {
		explicit vec2(float init_x = 0, float init_y = 0) : x(init_x), y(init_y) {}

		float x;
		float y;
	};
}

#endif // !_VEC2_HPP

