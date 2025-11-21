set(LIBRARY_NAME "descent-log")

add_library(${LIBRARY_NAME}
	log.c
	tables.c
)

target_include_directories(${LIBRARY_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${LIBRARY_NAME} PRIVATE
	descent-thread
)

target_enable_iwyu(${LIBRARY_NAME})
