CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(math)

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(INCLUDE_FILES src/Rect.hpp src/vec2.hpp src/vec3.hpp src/vec4.hpp src/ray3.hpp src/mat4.hpp src/matrix_transform.hpp src/Transform.hpp)
SET(SOURCE_FILES  src/vec2.cpp src/vec3.cpp src/mat4.cpp src/matrix_transform.cpp)

ADD_LIBRARY(math OBJECT ${INCLUDE_FILES} ${SOURCE_FILES})