#ifndef _RECT_HPP

#define _RECT_HPP
#include "vec2.hpp"

namespace math {
        /**
         * (x,y) top-left corner
         */
	struct Rect final {
		Rect();
		Rect(const float x, const float y, const float width, const float height) : 
                x(x), y(y), width(width), height(height) {}

        Rect(const vec2 & min, const vec2 & max);

        vec2    center() const;
        vec2    max() const;
        vec2    min() const;


		float   x;
		float   y;
		float   width;
		float   height;
	};
}
#endif // !_RECT_HPP
