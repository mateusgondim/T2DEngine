CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(2d_game)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

FIND_PACKAGE(glfw3 3.2 REQUIRED)
FIND_PACKAGE(OpenGL    REQUIRED)
FIND_PACKAGE(GLEW      REQUIRED)

#SOIL
FIND_LIBRARY(SOIL_LIBRARY SOIL)
FIND_PATH(SOIL_INCLUDE_DIR SOIL.h)

#CGM
FIND_LIBRARY(CGM-2.0_LIBRARY CGM-2.0)
FIND_PATH(CGM-2.0_INCLUDE_DIR vec2.hpp)

INCLUDE_DIRECTORIES( ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${SOIL_INCLUDE_DIR} ${CGM-2.0_INCLUDE_DIR})

SET(INCLUDE_FILES src/Shader.hpp src/Tile.hpp src/Tileset.hpp src/Tile_map.hpp src/Tile_map_renderer.hpp src/AABB_2d.hpp src/Game_object.hpp src/Renderable_game_object.hpp src/Actor.hpp src/State.hpp src/Player_idle_state.hpp src/Player_running_state.hpp src/Command.hpp src/Move_left_command.hpp src/Move_right_command.hpp src/Move_up_command.hpp src/Move_down_command.hpp src/Player.hpp src/Vertex1P1C1UV.hpp src/Sprite_batch.hpp src/tmx_parser.hpp src/game.hpp src/Button.hpp src/Input_handler.hpp src/Sprite.hpp src/Sprite_atlas.hpp src/Sprite_atlas_manager.hpp src/Animation.hpp src/Animation_player.hpp src/Timer.hpp src/Rect.hpp src/Texture_2d.hpp src/Texture_2d_manager.hpp)
SET(SOURCE_FILES  src/Shader.cpp src/Tileset.cpp src/Tile_map.cpp src/Tile_map_renderer.cpp src/Actor.cpp src/Texture_2d.cpp src/Texture_2d_manager.cpp src/Sprite.cpp src/Sprite_atlas.cpp src/Sprite_atlas_manager.cpp src/Animation_player.cpp src/Player_idle_state.cpp src/Player_running_state.cpp src/Timer.cpp src/Sprite_batch.cpp src/Button.cpp src/Input_handler.cpp src/Player.cpp src/tmx_parser.cpp src/game.cpp)

ADD_EXECUTABLE(2d_game ${INCLUDE_FILES} ${SOURCE_FILES})


TARGET_LINK_LIBRARIES(2d_game ${SOIL_LIBRARY} ${CGM-2.0_LIBRARY}  ${GLFW3_LIBRARY} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})