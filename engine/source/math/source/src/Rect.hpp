#ifndef _RECT_HPP

#define _RECT_HPP

namespace math {
        /**
         * (x,y) top-left corner
         */
	struct Rect final {
		Rect();
		Rect(const float x, const float y, const float width, const float height) : 
                x(x), y(y), width(width), height(height) {}


		float x;
		float y;
		float width;
		float height;
	};
}
#endif // !_RECT_HPP
