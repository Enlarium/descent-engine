set(EXECUTABLE_NAME "descent-test-cli")

add_executable(${EXECUTABLE_NAME}
	actions.c
	main.c
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
	descent-cli
	descent-utilities
)

add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})
