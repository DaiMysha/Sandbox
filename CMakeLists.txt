cmake_minimum_required (VERSION 2.8.8)

# define a macro that helps defining an option
project("Sandbox")

include(cmake/macro.cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(LIB_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CMAKE_SHARED_LIBRARY_PREFIX )

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules;${CMAKE_SOURCE_DIR}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules;${CMAKE_SOURCE_DIR}")

set_option(BUILD_BOX2DTEST FALSE BOOL "box2D test project")
set_option(BUILD_SAT FALSE BOOL "SAT implementation for SFML")

# add the subdirectories
if(BUILD_BOX2DTEST)
	add_subdirectory(boxTest)
endif()

if(BUILD_SAT)
	add_subdirectory(sat)
endif()

