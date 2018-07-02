CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(mem)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(INCLUDE_FILES src/memory_allocator.hpp src/Pool_allocator.hpp src/runtime_memory_allocator.hpp)
SET(SOURCE_FILES  src/memory_allocator.cpp src/Pool_allocator.cpp src/runtime_memory_allocator.cpp)

ADD_LIBRARY(mem OBJECT ${INCLUDE_FILES} ${SOURCE_FILES})