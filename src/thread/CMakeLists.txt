set(LIBRARY_NAME "descent-thread")

add_library(${LIBRARY_NAME}
	call_once.c
	condition.c
	futex.c
	mutex.c
	qutex.c
	#rwlock.c
	semaphore.c
	thread.c
	tid.c
)

target_compile_definitions(${LIBRARY_NAME} PRIVATE
	${DESCENT_DEFINITIONS}
)

target_compile_options(${LIBRARY_NAME} PRIVATE
	${DESCENT_FLAGS}
)

target_include_directories(${LIBRARY_NAME} PRIVATE
	${DESCENT_INCLUDE_DIRS}
)

target_link_libraries(${LIBRARY_NAME} PRIVATE
	descent-string
)

target_enable_iwyu(${LIBRARY_NAME})
