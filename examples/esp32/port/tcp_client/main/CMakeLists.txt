idf_component_register(SRCS tcp_client.cpp example_init.cpp
                       INCLUDE_DIRS
                       REQUIRES)

target_compile_options(${COMPONENT_LIB} 
						PRIVATE -std=gnu++17 
						-DESP_IDF_PLATAFORM=1 
						-DCOAP_TE_PORT_C_STANDARD=1
						-DCOAP_TE_USE_SELECT=1)
