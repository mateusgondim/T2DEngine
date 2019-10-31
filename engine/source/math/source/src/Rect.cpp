#include "Rect.hpp"
#include "vec2.hpp"

namespace math
{

        Rect::Rect() : x(0), y(0), width(0), height(0) {}

        Rect::Rect(const vec2 & min, const vec2 & max)
        {
                x = min.x;
                y = max.y;
                width = max.x - x;
                height = y - min.y;
        }

}