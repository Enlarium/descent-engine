set(EXECUTABLE_NAME "descent-exec")

add_executable(${EXECUTABLE_NAME}
	main.c
	cli.c
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
	${Vulkan_LIBRARIES}
	descent-alloc
	descent-cli
	descent-core
	descent-thread
	descent-rcode
)