CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(gfx)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(INCLUDE_FILES  src/Shader.hpp src/Texture_2d.hpp src/Vertex1P1C1UV.hpp src/Sprite_atlas.hpp  src/Sprite.hpp src/Animation.hpp src/Animation_player.hpp src/Animator_controller.hpp src/Animation_state.hpp src/Condition.hpp src/Int_condition.hpp src/Bool_condition.hpp src/Transition.hpp src/Sprite_batch.hpp src/Graphics_manager.hpp src/Camera_2d.hpp)
SET(SOURCE_FILES   src/Shader.cpp src/Texture_2d.cpp src/Sprite_atlas.cpp src/Sprite.cpp src/Animation_player.cpp src/Animator_controller.cpp src/Animation_state.cpp src/Int_condition.cpp src/Transition.cpp src/Sprite_batch.cpp src/Graphics_manager.cpp src/Camera_2d.cpp)

ADD_LIBRARY(gfx OBJECT ${INCLUDE_FILES} ${SOURCE_FILES})

TARGET_INCLUDE_DIRECTORIES(gfx PUBLIC ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${SOIL_INCLUDE_DIR} ${MATH_INCLUDE_DIR} ${G_UTILITY_INCLUDE_DIR})
