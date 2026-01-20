set(EXECUTABLE_NAME "descent-test-cli")

add_executable(${EXECUTABLE_NAME}
	actions.c
	main.c
)

target_compile_definitions(${LIBRARY_NAME} PRIVATE
	${DESCENT_FLAGS}
)

target_include_directories(${LIBRARY_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
	descent-cli
)

target_enable_iwyu(${LIBRARY_NAME})

add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})
