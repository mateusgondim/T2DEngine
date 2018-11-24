CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(gfx)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(INCLUDE_FILES  src/Shader.hpp src/Shader_manager.hpp src/Texture_2d.hpp src/Texture_2d_manager.hpp src/Vertex1P1C1UV.hpp src/Sprite_atlas.hpp  src/Sprite.hpp   src/Sprite_atlas_manager.hpp src/Animation.hpp src/Animation_player.hpp src/Animator_controller_parameter.hpp src/Animator_controller.hpp src/Animator_state.hpp src/Animator_state_transition.hpp src/Animator_condition src/Sprite_batch.hpp src/Graphics_manager.hpp)
SET(SOURCE_FILES   src/Shader.cpp src/Shader_manager.cpp src/Texture_2d.cpp src/Texture_2d_manager.cpp src/Sprite_atlas.cpp src/Sprite.cpp  src/Sprite_atlas_manager.cpp src/Animation_player.cpp src/Animator_controller_parameter.cpp src/Animator_controller.cpp src/Animator_state.cpp src/Animator_state_transition.cpp src/Sprite_batch.cpp src/Graphics_manager.cpp)

ADD_LIBRARY(gfx OBJECT ${INCLUDE_FILES} ${SOURCE_FILES})

TARGET_INCLUDE_DIRECTORIES(gfx PUBLIC ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${STB_INCLUDE_DIR} ${MATH_INCLUDE_DIR} ${MEM_INCLUDE_DIR} ${UTILITY_INCLUDE_DIR} ${TMAP_INCLUDE_DIR} ${RMS_INCLUDE_DIR})
