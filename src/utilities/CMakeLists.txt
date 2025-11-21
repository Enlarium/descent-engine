set(LIBRARY_NAME "descent-utilities")

add_library(${LIBRARY_NAME}
	codes.c
)

target_include_directories(${LIBRARY_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${LIBRARY_NAME} PRIVATE
)

target_enable_iwyu(${LIBRARY_NAME})
