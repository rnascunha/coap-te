message(STATUS "Compile examples...")

set(EXAMPLES_DIR ./examples)

set(EXAMPLES_LIST	
				${EXAMPLES_DIR}/internal/percent_encode.cpp
				${EXAMPLES_DIR}/internal/tree.cpp
				${EXAMPLES_DIR}/internal/percent_decode.cpp
				${EXAMPLES_DIR}/message/factory.cpp
				${EXAMPLES_DIR}/message/serialize_parse.cpp
				${EXAMPLES_DIR}/message/option.cpp
				${EXAMPLES_DIR}/message/serialize_class.cpp
				${EXAMPLES_DIR}/message/serialize_parse_reliable.cpp
				${EXAMPLES_DIR}/message/signaling.cpp
				${EXAMPLES_DIR}/message/no_response.cpp	
				${EXAMPLES_DIR}/uri/decompose.cpp
				${EXAMPLES_DIR}/resource/discovery.cpp
				${EXAMPLES_DIR}/transmission/raw_transaction.cpp
				${EXAMPLES_DIR}/transmission/raw_engine.cpp
				${EXAMPLES_DIR}/transmission/engine_server.cpp
				${EXAMPLES_DIR}/transmission/request_get_block_wise.cpp
				${EXAMPLES_DIR}/transmission/request_put_block_wise.cpp
				${EXAMPLES_DIR}/transmission/response_block_wise.cpp
				${EXAMPLES_DIR}/transmission/engine_tcp_client.cpp
				${EXAMPLES_DIR}/transmission/engine_tcp_server.cpp
				${EXAMPLES_DIR}/observe/client_observe.cpp
				${EXAMPLES_DIR}/observe/server_observe.cpp
				${EXAMPLES_DIR}/observe/tcp_client_observe.cpp
				${EXAMPLES_DIR}/observe/tcp_server_observe.cpp
				${EXAMPLES_DIR}/port/endpoint_ipv6.cpp
				${EXAMPLES_DIR}/port/udp_server.cpp
				${EXAMPLES_DIR}/port/udp_client.cpp
				${EXAMPLES_DIR}/port/async_tcp_client.cpp
				${EXAMPLES_DIR}/port/tcp_client.cpp
				${EXAMPLES_DIR}/port/tcp_server.cpp
			)

#List of examples that must link to network at emscripten
list(APPEND emscripten_net_list raw_transaction
								raw_engine
								engine_server
								request_get_block_wise
								request_put_block_wise
								response_block_wise
								engine_tcp_client
								engine_tcp_server
								client_observe
								server_observe
								endpoint_ipv6
								udp_server
								udp_client
								tcp_client
								tcp_server)
								
#List of examples that must link to pthreads at linux
list(APPEND unix_pthread_list engine_server
								engine_tcp_server
								server_observe
								tcp_server_observe)
								
if(EMSCRIPTEN)
	if(NOT DEFINED WASM_OUTPUT_HTML OR WASM_OUTPUT_HTML EQUAL 1)
		set(CMAKE_EXECUTABLE_SUFFIX ".html")
	endif()
endif()
					
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
	
	#Particular system dependencies
	if(EMSCRIPTEN)
		if(${EXAMPLE_OUT} IN_LIST emscripten_net_list)
			target_link_libraries(${EXAMPLE_OUT} "-lwebsocket.js -s PROXY_POSIX_SOCKETS=1 -s USE_PTHREADS=1 -s PROXY_TO_PTHREAD=1")
			target_link_libraries(${EXAMPLE_OUT} pthread)
		endif()
		if(NOT DEFINED WASM_EMRUN_ENABLE OR WASM_EMRUN_ENABLE EQUAL 1)
			target_link_libraries(${EXAMPLE_OUT} "--emrun")
		endif()
	elseif(WIN32)
		if(MSVC)
			target_compile_features(${EXAMPLE_OUT} PUBLIC cxx_std_20)
		endif()
		target_link_libraries(${EXAMPLE_OUT} wsock32 ws2_32)
	else()
		if(${EXAMPLE_OUT} IN_LIST unix_pthread_list)
			target_link_libraries(${EXAMPLE_OUT} pthread)
		endif()
	endif()
	
	#Linking example to CoAP-te library
	target_link_libraries(${EXAMPLE_OUT} ${PROJECT_NAME})

endforeach()