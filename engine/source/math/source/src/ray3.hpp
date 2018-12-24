#ifndef _RAY3_HPP
#define _RAY3_HPP
#include "vec2.hpp"
#include "vec3.hpp"

namespace math
{
        struct ray3 {
                ray3()
                {
                        origin.zero();
                        direction.zero();
                }
                
                ray3(const vec3 & o, const vec3 & dir) : origin(o), direction(dir) {}


                vec3    origin;
                vec3    direction;
        };
}
#endif // !_RAY3_HPP