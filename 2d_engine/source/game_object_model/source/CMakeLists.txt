CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(gom)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(INCLUDE_FILES  src/Game_object.hpp  src/Actor.hpp src/Creator.hpp src/Gameplay_state.hpp src/Game_object_handle.hpp src/Game_object_manager.hpp)
SET(SOURCE_FILES  src/Game_object.cpp   src/Actor.cpp src/Creator.cpp src/Game_object_handle.cpp src/Game_object_manager.cpp)

ADD_LIBRARY(gom OBJECT ${INCLUDE_FILES} ${SOURCE_FILES})
TARGET_INCLUDE_DIRECTORIES( gom PUBLIC ${MATH_INCLUDE_DIR} ${MEM_INCLUDE_DIR} ${RMS_INCLUDE_DIR} ${UTILITY_INCLUDE_DIR} ${GFX_INCLUDE_DIR} ${PHY_2D_INCLUDE_DIR})
