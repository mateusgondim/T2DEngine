CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(tmap)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(INCLUDE_FILES src/Tile.hpp src/Tileset.hpp src/Tile_map.hpp src/tmx_parser.hpp)
SET(SOURCE_FILES  src/Tileset.cpp src/Tile_map.cpp src/tmx_parser.cpp)

ADD_LIBRARY(tmap OBJECT ${INCLUDE_FILES} ${SOURCE_FILES})
INCLUDE_DIRECTORIES(tmap ${MATH_INCLUDE_DIR}  ${UTILITY_INCLUDE_DIR})

