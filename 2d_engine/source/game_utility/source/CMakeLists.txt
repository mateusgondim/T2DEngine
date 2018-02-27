CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(g_utility)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")


#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(INCLUDE_FILES src/Timer.hpp src/memory_allocator.hpp src/Pool_allocator.hpp src/crc32.hpp src/string_id.hpp)
SET(SOURCE_FILES  src/Timer.cpp src/memory_allocator.cpp src/Pool_allocator.cpp src/crc32.cpp src/string_id.cpp)

INCLUDE_DIRECTORIES( ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
ADD_LIBRARY(g_utility OBJECT ${INCLUDE_FILES} ${SOURCE_FILES})

