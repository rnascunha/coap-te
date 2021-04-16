include (${COMPONENT_DIR}/files.cmake)

list(APPEND MAIN_SRC ${SRC_DIR_PORT}/c_standard_port.cpp)

idf_component_register(SRCS ${MAIN_SRC}
                       INCLUDE_DIRS ${MAIN_INCLUDE_DIRS}
                       REQUIRES)

target_compile_options(${COMPONENT_LIB} 
						PRIVATE -std=c++17 
						-DESP_IDF_PLATAFORM=1 
						-DCOAP_TE_PORT_C_STANDARD=1
						-DCOAP_TE_USE_SELECT=1)