# This script locates the UTILS library.
#
# USAGE
# find_package( Box2D )
#
# If Box2D is not installed in a standard path, you can use
# Box2D_ROOT CMake (or environment) variables to tell CMake where to look for
# Box2D.
#
#
# OUTPUT
#
# This script defines the following variables:
#   - Box2D_FOUND:           true if the Box2D library is found
#   - Box2D_INCLUDE_DIR:     the path where Box2D headers are located
#
#
# EXAMPLE
#
# find_package( Box2D REQUIRED )
# include_directories( ${Box2D_INCLUDE_DIR} )
# add_executable( myapp ... )
# target_link_libraries( myapp ${Box2D_LIBRARY} ... )

set( Box2D_FOUND false )

find_path(
	Box2D_INCLUDE_DIR
	Box2D/Box2D.h
	PATH_SUFFIXES
		include
	PATHS
		/usr
		/usr/local
		${Box2DDIR}
		${Box2D_ROOT}
		$ENV{Box2D_ROOT}
		$ENV{Box2DDIR}
)

find_library(
	Box2D_LIBRARY
	Box2D
	PATH_SUFFIXES
		lib
		lib64
	PATHS
		/usr
		/usr/local
		${Box2DDIR}
		${Box2D_ROOT}
		$ENV{Box2D_ROOT}
		$ENV{Box2DDIR}
)

if( NOT Box2D_INCLUDE_DIR OR NOT Box2D_LIBRARY )
	message( FATAL_ERROR "Box2D not found. Set Box2D_ROOT to the installation root directory (containing inculude/ and lib/)" )
else()
	message( STATUS "Box2D found: ${Box2D_INCLUDE_DIR}" )
	set(Box2D_LIBRARY ${Box2D_LIBRARY}/lib/Box2D.lib)
endif()
