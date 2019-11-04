CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(engine)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(cmake/preprocessor.cmake)
include(cmake/util.cmake)

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL    REQUIRED)
find_package(GLEW      REQUIRED)

# STB_IMAGE
find_path(STB_INCLUDE_DIR NAMES stb_image.h HINTS ${CMAKE_CURRENT_LIST_DIR}/stb_image)

# MATH
find_path(MATH_INCLUDE_DIR NAMES vec2.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/math/source/src)

# MEM
find_path(MEM_INCLUDE_DIR NAMES memory_allocator.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/mem/source/src)

# UTILITY
find_path(UTILITY_INCLUDE_DIR NAMES Timer.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/utility/source/src )

# RMS
find_path(RMS_INCLUDE_DIR NAMES Resource.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/rms/source/src )

# GRAPHICS_SYSTEM
find_path(GFX_INCLUDE_DIR NAMES texture_2d.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/gfx/source/src)

# INPUT SYSTEM
find_path(IO_INCLUDE_DIR NAMES Input_manager.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/io/source/src)

# TILE MAP
find_path(TMAP_INCLUDE_DIR NAMES Tile.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/tmap/source/src)

# PHYSISCS_SYSTEM
find_path(PHY_2D_INCLUDE_DIR NAMES Body_2d.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/phy_2d/source/src)

# GAME OBJECT MODEL
find_path(GOM_INCLUDE_DIR NAMES Game_object.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/gom/source/src)

# UI
find_path(UI_INCLUDE_DIR NAMES Widget.hpp HINTS ${CMAKE_CURRENT_LIST_DIR}/ui/source/src)

# LEVEL MANAGEMENT
find_path(LEVEL_MANAGEMENT_INCLUDE_DIR 
          NAMES Level_manager.hpp 
	  HINTS ${CMAKE_CURRENT_LIST_DIR}/level_management/source/src)

add_subdirectory(math/source)
add_subdirectory(utility/source)
add_subdirectory(sid_preprocessor/source)
add_subdirectory(mem/source)
add_subdirectory(rms/source)
add_subdirectory(io/source)
add_subdirectory(gfx/source)
add_subdirectory(tmap/source)
add_subdirectory(phy_2d/source)
add_subdirectory(gom/source)
add_subdirectory(ui/source)
add_subdirectory(level_management/source)

set(include_files  src/engine.hpp src/Engine_collision_listener.hpp)
set(source_files   src/engine.cpp src/Engine_collision_listener.cpp)

add_library(engine ${include_files} ${source_files} $<TARGET_OBJECTS:gfx> $<TARGET_OBJECTS:math> 
                   $<TARGET_OBJECTS:mem> $<TARGET_OBJECTS:io> $<TARGET_OBJECTS:utility> 
		   $<TARGET_OBJECTS:rms> $<TARGET_OBJECTS:tmap> $<TARGET_OBJECTS:phy_2d> 
		   $<TARGET_OBJECTS:gom> $<TARGET_OBJECTS:ui> $<TARGET_OBJECTS:level_management>)

target_include_directories(engine PUBLIC ${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} 
                                          ${OPENGL_INCLUDE_DIR} ${GFX_INCLUDE_DIR} 
					  ${MATH_INCLUDE_DIR} ${MEM_INCLUDE_DIR} 
					  ${IO_INCLUDE_DIR} ${UTILITY_INCLUDE_DIR} 
					  ${RMS_INCLUDE_DIR} ${TMAP_INCLUDE_DIR} 
					  ${PHY_2D_INCLUDE_DIR} ${GOM_INCLUDE_DIR}
					  ${UI_INCLUDE_DIR} ${LEVEL_MANAGEMENT_INCLUDE_DIR})

