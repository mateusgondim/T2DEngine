cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)
project(sid_preprocessor VERSION 0.1 LANGUAGES CXX)

set(include_files src/main.hpp)
set(source_files src/main.cpp)

add_executable(sid_preprocessor ${include_files} ${source_files})

target_include_directories(sid_preprocessor PUBLIC 
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
			   $<INSTALL_INTERFACE:include>
			   PRIVATE src ${UTILITY_INCLUDE_DIR} )

target_link_libraries(sid_preprocessor $<TARGET_OBJECTS:utility>)
export(TARGETS sid_preprocessor FILE SidPreprocessorConfig.cmake)


