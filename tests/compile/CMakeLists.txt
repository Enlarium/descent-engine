set(EXECUTABLE_NAME "descent-test-compile")

add_executable(${EXECUTABLE_NAME}
	main.c
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
	descent-thread
	descent-alloc
)

add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})
