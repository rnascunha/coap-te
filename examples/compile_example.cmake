message(STATUS "Compile examples...")

set(EXAMPLES_DIR ./examples)

set(EXAMPLES_LIST	
#					${EXAMPLES_DIR}/coap_server.cpp
#					${EXAMPLES_DIR}/internal/percent_encode.cpp
#					${EXAMPLES_DIR}/internal/tree.cpp
					${EXAMPLES_DIR}/internal/percent_decode.cpp
					${EXAMPLES_DIR}/message/factory.cpp
					${EXAMPLES_DIR}/message/serialize_parse.cpp
					${EXAMPLES_DIR}/message/option.cpp
					${EXAMPLES_DIR}/uri/decompose.cpp
#					${EXAMPLES_DIR}/port/udp_socket.cpp
#					${EXAMPLES_DIR}/transmission/raw_engine.cpp
#					${EXAMPLES_DIR}/transmission/raw_transaction.cpp
					)

foreach(example ${EXAMPLES_LIST})
	string(REGEX REPLACE "./.*/" "" EXAMPLE_OUT ${example})
	string(REGEX REPLACE "\.cpp$" "" EXAMPLE_OUT ${EXAMPLE_OUT})
	message("Compiling: ${EXAMPLE_OUT}")

	add_executable(${EXAMPLE_OUT} ${example})
	set_target_properties(${EXAMPLE_OUT} PROPERTIES
	    CXX_STANDARD 17
	    CXX_STANDARD_REQUIRED ON
	    CXX_EXTENSIONS ON
	)
	
	target_link_libraries(${EXAMPLE_OUT} ${PROJECT_NAME})
	
	if(WIN32)
		target_link_libraries(${EXAMPLE_OUT} wsock32 ws2_32)
	else()
		if(${EXAMPLE_OUT} STREQUAL coap_server)
			target_link_libraries(${EXAMPLE_OUT} pthread)
		endif()
	endif()

endforeach()