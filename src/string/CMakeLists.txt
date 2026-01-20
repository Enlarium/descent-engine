set(LIBRARY_NAME "descent-string")

add_library(${LIBRARY_NAME}
	nchar.c
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

target_link_libraries(${LIBRARY_NAME} PRIVATE
)

target_enable_iwyu(${LIBRARY_NAME})
