CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(io)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(INCLUDE_FILES src/Input_manager.hpp src/Abstract_keyboard_index.hpp src/Input_abstraction_layer.hpp src/Keyboard_state.hpp src/Keyboard_button_mapper.hpp)
SET(SOURCE_FILES  src/Input_manager.cpp src/Input_abstraction_layer.cpp src/Keyboard_state.cpp)


ADD_LIBRARY(io OBJECT ${INCLUDE_FILES} ${SOURCE_FILES})
TARGET_INCLUDE_DIRECTORIES( io PUBLIC ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} )



