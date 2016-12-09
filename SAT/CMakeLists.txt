cmake_minimum_required (VERSION 2.8.8)

# define a macro that helps defining an option

# project name
set(PROJECT_NAME "SAT")
project (${PROJECT_NAME})

include(cmake/macro.cmake)

set(LIBS "")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(LIB_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CMAKE_SHARED_LIBRARY_PREFIX )

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules;${CMAKE_SOURCE_DIR}")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set_option(SFML_ROOT ${CMAKE_INSTALL_PREFIX}/../SFML PATH "The SFML root installation")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set_option(SFML_ROOT ${CMAKE_INSTALL_PREFIX}/SFML PATH "The SFML root installation")
else()
	set_option(SFML_ROOT ${CMAKE_INSTALL_PREFIX}/SFML PATH "The SFML root installation")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules;${CMAKE_SOURCE_DIR}")

find_package(SFML 2 COMPONENTS system graphics window REQUIRED)
if(NOT SFML_FOUND)
	message(FATAL_ERROR "SFML Modules not found. Please set the SFML_ROOT variable to your SFML root installation directory and retry")
endif()


include_directories(${SFML_INCLUDE_DIR})

list(APPEND LIBS
	${LIBS}
	${SFML_LIBRARIES}
	${SFML_DEPENDENCIES}
)

# add the subdirectories
add_subdirectory(sat)

