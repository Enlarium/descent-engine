set(LIBRARY_NAME "descent-logging")

add_library(${LIBRARY_NAME}
	logging.c
	tables.c
)

target_include_directories(${LIBRARY_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${LIBRARY_NAME} PRIVATE
	descent-thread
)
