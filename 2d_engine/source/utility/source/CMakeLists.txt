CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(utility)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")


#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(INCLUDE_FILES src/Timer.hpp src/crc32.hpp src/string_id.hpp src/Path.hpp)
SET(SOURCE_FILES  src/Timer.cpp src/crc32.cpp src/string_id.cpp src/Path.cpp)

INCLUDE_DIRECTORIES( ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
ADD_LIBRARY(utility OBJECT ${INCLUDE_FILES} ${SOURCE_FILES})

