set(EXAMPLES_DIR    "examples")
set(EXAMPLES        "option_table")
set(OUTPUT_EXAMPLE	${CMAKE_BINARY_DIR}/examples)

foreach(EXAMPLE ${EXAMPLES})
	add_executable(${EXAMPLE} ${EXAMPLES_DIR}/${EXAMPLE}.cpp)
	target_link_libraries(${EXAMPLE} PUBLIC ${PROJECT_NAME})

	set_target_properties(${EXAMPLE} PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_EXAMPLE})

    target_compile_options(${EXAMPLE} PUBLIC -fexceptions)
endforeach()