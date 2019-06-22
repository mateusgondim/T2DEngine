cmake_minimum_required(VERSION 2.6.4)
project(sid_preprocessor)

set(include_files src/main.hpp)
set(source_files src/main.cpp)

add_executable(sid_preprocessor ${include_files} ${source_files} $<TARGET_OBJECTS:utility>)

target_include_directories(sid_preprocessor PUBLIC 
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
			   $<INSTALL_INTERFACE:include>
			   PRIVATE src ${UTILITY_INCLUDE_DIR} )
target_link_libraries(sid_preprocessor ${GLFW3_LIBRARY})
export(TARGETS sid_preprocessor FILE sid_preprocessorConfig.cmake)
export(PACKAGE sid_preprocessor)


