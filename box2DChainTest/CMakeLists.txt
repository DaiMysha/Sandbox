cmake_minimum_required (VERSION 2.8.8)

# define a macro that helps defining an option

# project name
set(PROJECT_NAME "BoxTest")
project (${PROJECT_NAME})

set(LIBS "")

find_package(SFML 2 COMPONENTS system graphics window REQUIRED)
if(NOT SFML_FOUND)
	message(FATAL_ERROR "SFML Modules not found. Please set the SFML_ROOT variable to your SFML root installation directory and retry")
endif()

find_package(BOX2D REQUIRED)

include_directories(${SFML_INCLUDE_DIR})
include_directories(${Box2D_INCLUDE_DIR})

list(APPEND LIBS
	${LIBS}
	${SFML_LIBRARIES}
	${SFML_DEPENDENCIES}
	${Box2D_LIBRARY}
)

# add the subdirectories
add_subdirectory(example)

