idf_component_register(SRCS engine_tcp_server.cpp example_init.cpp
                       INCLUDE_DIRS
                       REQUIRES)

target_compile_options(${COMPONENT_LIB} 
						PRIVATE -std=gnu++17 
						)
