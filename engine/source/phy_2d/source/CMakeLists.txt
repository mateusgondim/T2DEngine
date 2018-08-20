CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(phy_2d)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(INCLUDE_FILES src/AABB_2d src/Body_2d.hpp src/Body_2d_def.hpp src/Collider_2d_def.hpp src/Collider_2d.hpp src/Collision_listener.hpp src/Broadphase.hpp src/N_squared_broadphase.hpp src/World.hpp src/Physics_manager.hpp)
SET(SOURCE_FILES  src/Body_2d.cpp src/Collider_2d.cpp src/Broadphase.cpp src/N_squared_broadphase.cpp src/World.cpp src/Physics_manager.cpp)

ADD_LIBRARY(phy_2d OBJECT ${INCLUDE_FILES} ${SOURCE_FILES})
TARGET_INCLUDE_DIRECTORIES(phy_2d PUBLIC ${MATH_INCLUDE_DIR} ${TMAP_INCLUDE_DIR} ${UTILITY_INCLUDE_DIR} ${MEM_INCLUDE_DIR})

