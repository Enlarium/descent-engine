set(LIBRARY_NAME "descent-thread")

add_library(${LIBRARY_NAME}
	thread.c
)

target_include_directories(${LIBRARY_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${LIBRARY_NAME} PRIVATE
	descent-utilities
)
