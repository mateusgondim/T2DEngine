CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

Project(ui)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#Using c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(source_dir src)

set(include_files src/Widget.hpp src/Canvas.hpp src/UI_manager.hpp)

set(unprocessed_source_files Widget.cpp Canvas.cpp UI_manager.cpp)

set(preprocessed_files "")
preprocess_source_files("${unprocessed_source_files}" preprocessed_files
                        "${CMAKE_CURRENT_LIST_DIR}/${source_dir}" "${CMAKE_CURRENT_BINARY_DIR}")

source_group(TREE "${CMAKE_CURRENT_BINARY_DIR}" PREFIX "Preprocessed Source Files"
             FILES ${preprocessed_files})

# add the directory to each source file name
set(full_unprocessed_sources_name "")
prepend_string_to_list("${source_dir}/" "${unprocessed_source_files}" full_unprocessed_sources_name)

# dont compile the unprocessed files
set_source_files_properties(${full_unprocessed_sources_name} PROPERTIES HEADER_FILE_ONLY TRUE)


INCLUDE_DIRECTORIES(${GLFW3_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
ADD_LIBRARY(ui OBJECT ${include_files} ${preprocessed_files} 
                       ${full_unprocessed_sources_name})

add_dependencies(ui sid_preprocessor)
add_dependencies(ui gom)

TARGET_INCLUDE_DIRECTORIES(ui PUBLIC "${CMAKE_CURRENT_LIST_DIR}/${source_dir}" 
                                       ${MATH_INCLUDE_DIR} ${MEM_INCLUDE_DIR}
                                       ${RMS_INCLUDE_DIR} ${UTILITY_INCLUDE_DIR}
				       ${IO_INCLUDE_DIR} ${GFX_INCLUDE_DIR}
				       ${PHY_2D_INCLUDE_DIR} ${GOM_INCLUDE_DIR})

export(TARGETS ui FILE uiConfig.cmake)