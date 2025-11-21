set(LIBRARY_NAME "descent-cli")

add_library(${LIBRARY_NAME}
	cli.c
)

target_include_directories(${LIBRARY_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${LIBRARY_NAME} PRIVATE
	descent-log
)

target_enable_iwyu(${LIBRARY_NAME})
