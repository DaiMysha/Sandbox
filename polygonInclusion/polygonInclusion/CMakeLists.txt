set(INCROOT ${PROJECT_SOURCE_DIR}/polygonInclusion)
set(SRCROOT ${PROJECT_SOURCE_DIR}/polygonInclusion)

set(FILES_HEADER
)

set(FILES_SRC
	${SRCROOT}/main.cpp
)
	
add_executable (${PROJECT_NAME}
	${FILES_HEADER}
	${FILES_SRC}
)
target_link_libraries (${PROJECT_NAME} ${LIBS})