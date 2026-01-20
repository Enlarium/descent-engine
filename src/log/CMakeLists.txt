set(LIBRARY_NAME "descent-log")

add_library(${LIBRARY_NAME}
	log.c
	tables.c
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
	descent-thread
)

target_enable_iwyu(${LIBRARY_NAME})
