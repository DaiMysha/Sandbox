set(INCROOT ${PROJECT_SOURCE_DIR}/${PROJECT_NAME})
set(SRCROOT ${PROJECT_SOURCE_DIR}/${PROJECT_NAME})

set(FILES_HEADER
	${SRCROOT}/QuadTree.hpp
	${SRCROOT}/QuadTree.tpl
)

set(FILES_SRC
	${SRCROOT}/main.cpp
)
	
add_executable (${PROJECT_NAME}
	${FILES_HEADER}
	${FILES_SRC}
)
add_executable (${PROJECT_NAME}-benchmark
	${FILES_HEADER}
	benchmark.cpp
)
target_link_libraries (${PROJECT_NAME} ${LIBS})
target_link_libraries (${PROJECT_NAME}-benchmark)