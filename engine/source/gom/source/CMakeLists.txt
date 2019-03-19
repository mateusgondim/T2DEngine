CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(gom)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(INCLUDE_FILES  src/Game_object.hpp src/Projectile.hpp src/Projectile_manager.hpp  src/Actor.hpp src/Enemy.hpp src/Game_object_data.hpp src/Creator.hpp src/Camera_2d.hpp src/Gameplay_state.hpp src/Game_object_handle.hpp src/Game_object_manager.hpp src/Level_manager.hpp)
SET(SOURCE_FILES  src/Game_object.cpp  src/Projectile.cpp src/Projectile_manager.cpp  src/Actor.cpp src/Enemy.cpp src/Creator.cpp src/Camera_2d.cpp src/Game_object_handle.cpp src/Game_object_manager.cpp src/Level_manager.cpp)

INCLUDE_DIRECTORIES(${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
ADD_LIBRARY(gom OBJECT ${INCLUDE_FILES} ${SOURCE_FILES})
TARGET_INCLUDE_DIRECTORIES( gom PUBLIC ${MATH_INCLUDE_DIR} ${MEM_INCLUDE_DIR} ${RMS_INCLUDE_DIR} ${UTILITY_INCLUDE_DIR} ${TMAP_INCLUDE_DIR} ${IO_INCLUDE_DIR} ${GFX_INCLUDE_DIR} ${PHY_2D_INCLUDE_DIR})
