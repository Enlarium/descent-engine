set(LIBRARY_NAME "descent-script")

add_library(${LIBRARY_NAME}
	script.c
	wrapper/log.c
)

target_include_directories(${LIBRARY_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${LIBRARY_NAME} PRIVATE
	${LUA_LIBRARIES}
	descent-log
)

target_enable_iwyu(${LIBRARY_NAME})
