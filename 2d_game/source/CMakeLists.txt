CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(game)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

FIND_PACKAGE(glfw3 3.2 REQUIRED)
FIND_PACKAGE(OpenGL    REQUIRED)
FIND_PACKAGE(GLEW      REQUIRED)

#SOIL
FIND_PATH(SOIL_INCLUDE_DIR NAMES SOIL.h HINTS ${CMAKE_CURRENT_LIST_DIR}/../../2d_engine/source/Simple\ OpenGL\ Image\ Library/src)
FIND_LIBRARY(SOIL_LIBRARY  NAMES SOIL   HINTS ${CMAKE_CURRENT_LIST_DIR}/../../2d_engine/source/Simple\ OpenGL\ Image\ Library/projects/vc9/debug)

#GRAPHICS_SYSTEM
FIND_PATH(TGS_INCLUDE_DIR NAMES texture_2d.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/../../2d_engine/source/tgs/source/src)

#G_UTILITY
FIND_PATH(G_UTILITY_INCLUDE_DIR NAMES Timer.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/../../2d_engine/source/game_utility/source/src )

#CGM(MATH)
FIND_PATH(CGM-2.0_INCLUDE_DIR NAMES vec2.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/../../2d_engine/source/cgm-2.0/source/src)

#INPUT SYSTEM
FIND_PATH(IO_INCLUDE_DIR NAMES Button.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/../../2d_engine/source/io/source/src)

#TILE MAP
FIND_PATH(TMAP_INCLUDE_DIR NAMES Tile.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/../../2d_engine/source/tmap/source/src)

#PHYSISCS_SYSTEM
FIND_PATH(PHY_2D_INCLUDE_DIR NAMES Body_2d.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/../../2d_engine/source/phy_2d/source/src)

#GAME OBJECT MODEL
FIND_PATH(GOM_INCLUDE_DIR NAMES game_object.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/../../2d_engine/source/game_object_model/source/src)

#ENGINE
FIND_LIBRARY(ENGINE_LIBRARY engine)
FIND_PATH(ENGINE_INCLUDE_DIR Engine.hpp)


SET(INCLUDE_FILES src/Player.hpp src/Player_idle_state.hpp src/Player_ducking_state.hpp src/Player_running_state.hpp src/Player_jumping_state.hpp src/Player_climbing_state.hpp src/Game_coll_listener.hpp src/game.hpp)
SET(SOURCE_FILES src/Player.cpp src/Player_idle_state.cpp  src/Player_ducking_state.cpp src/Player_running_state.cpp src/Player_jumping_state.cpp src/Player_climbing_state.cpp src/Game_coll_listener.cpp src/game.cpp)

ADD_EXECUTABLE(game ${INCLUDE_FILES} ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(game ${ENGINE_LIBRARY} ${GLFW3_LIBRARY} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY} ${SOIL_LIBRARY} )
TARGET_INCLUDE_DIRECTORIES(game PUBLIC ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${TGS_INCLUDE_DIR} ${CGM-2.0_INCLUDE_DIR} ${IO_INCLUDE_DIR} ${G_UTILITY_INCLUDE_DIR} ${TMAP_INCLUDE_DIR} ${PHY_2D_INCLUDE_DIR} ${GOM_INCLUDE_DIR} ${ENGINE_INCLUDE_DIR})

