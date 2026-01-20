set(LIBRARY_NAME "descent-file")

add_library(${LIBRARY_NAME}
	dir.c
	file.c
	file2.c
	fobj.c
	path.c
)

target_compile_definitions(${LIBRARY_NAME} PRIVATE
	${DESCENT_DEFINITIONS}
)

target_compile_options(${LIBRARY_NAME} PRIVATE
	${DESCENT_FLAGS}
)

target_include_directories(${LIBRARY_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
	descent-alloc
)

target_enable_iwyu(${LIBRARY_NAME})
