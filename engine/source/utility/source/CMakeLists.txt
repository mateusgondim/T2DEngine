CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(utility)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")


#Using c++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(include_files src/Timer.hpp src/crc32.hpp src/string_id.hpp src/Path.hpp src/Variant.hpp
                  src/Event_arguments.hpp src/Event.hpp)

set(source_files  src/Timer.cpp src/crc32.cpp src/string_id.cpp src/Path.cpp src/Variant.cpp
                  src/Event_arguments.cpp)

include_directories( ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
add_library(utility OBJECT ${include_files} ${source_files})

