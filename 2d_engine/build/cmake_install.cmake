# Install script for directory: C:/Users/Mateus/Documents/GitHub/Demos/2d_engine/source

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/engine")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/Mateus/Documents/GitHub/Demos/2d_engine/build/math/source/cmake_install.cmake")
  include("C:/Users/Mateus/Documents/GitHub/Demos/2d_engine/build/game_utility/source/cmake_install.cmake")
  include("C:/Users/Mateus/Documents/GitHub/Demos/2d_engine/build/mem/source/cmake_install.cmake")
  include("C:/Users/Mateus/Documents/GitHub/Demos/2d_engine/build/rms/source/cmake_install.cmake")
  include("C:/Users/Mateus/Documents/GitHub/Demos/2d_engine/build/io/source/cmake_install.cmake")
  include("C:/Users/Mateus/Documents/GitHub/Demos/2d_engine/build/gfx/source/cmake_install.cmake")
  include("C:/Users/Mateus/Documents/GitHub/Demos/2d_engine/build/tmap/source/cmake_install.cmake")
  include("C:/Users/Mateus/Documents/GitHub/Demos/2d_engine/build/phy_2d/source/cmake_install.cmake")
  include("C:/Users/Mateus/Documents/GitHub/Demos/2d_engine/build/game_object_model/source/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/Mateus/Documents/GitHub/Demos/2d_engine/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
