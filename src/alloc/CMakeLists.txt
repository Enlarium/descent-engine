set(LIBRARY_NAME "descent-alloc")

add_library(${LIBRARY_NAME}
	sysalloc.c
)

target_include_directories(${LIBRARY_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${LIBRARY_NAME} PRIVATE
)
