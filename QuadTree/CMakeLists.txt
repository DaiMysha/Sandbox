cmake_minimum_required (VERSION 2.8.8)

# define a macro that helps defining an option

# project name
set(PROJECT_NAME "QuadTree")
project (${PROJECT_NAME})

set(LIBS "")

find_package(SFML 2 COMPONENTS system graphics window REQUIRED)
if(NOT SFML_FOUND)
	message(FATAL_ERROR "SFML Modules not found. Please set the SFML_ROOT variable to your SFML root installation directory and retry")
endif()

find_package(DMUtils)
if(NOT DMUTILS_FOUND)
	message(FATAL_ERROR "DMutils not found. Please set the DMUTILS_ROOT variable to your DMUtils root installation directory and retry")
endif()

include_directories(${SFML_INCLUDE_DIR} ${DMUTILS_INCLUDE_DIR})

list(APPEND LIBS
	${LIBS}
	${SFML_LIBRARIES}
	${SFML_DEPENDENCIES}
)

# add the subdirectories
add_subdirectory(${PROJECT_NAME})

