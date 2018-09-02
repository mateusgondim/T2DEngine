CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(tmap)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(INCLUDE_FILES src/Property.hpp src/Tile_layer.hpp src/Tile.hpp src/Object_group.hpp src/Object.hpp src/Tileset.hpp src/Tile_map.hpp )
SET(SOURCE_FILES  src/Property.cpp src/Tile_layer.cpp src/Tileset.cpp src/Object_group.cpp src/Object.cpp src/Tile_map.cpp )

ADD_LIBRARY(tmap OBJECT ${INCLUDE_FILES} ${SOURCE_FILES})
INCLUDE_DIRECTORIES(tmap ${MATH_INCLUDE_DIR}  ${UTILITY_INCLUDE_DIR})

