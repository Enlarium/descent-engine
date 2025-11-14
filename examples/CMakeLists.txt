set(EXECUTABLE_NAME "descent-exec")

add_executable(${EXECUTABLE_NAME}
	main.c
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
	${LUA_LIBRARIES}
	${Vulkan_LIBRARIES}
	descent-alloc
	descent-thread
	descent-logging
)