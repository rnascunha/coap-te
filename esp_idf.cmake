include (${COMPONENT_DIR}/files.cmake)

list(APPEND MAIN_SRC ${SRC_DIR_PORT}/c_standard_port.cpp)

idf_component_register(SRCS ${MAIN_SRC}
                       INCLUDE_DIRS ${MAIN_INCLUDE_DIRS}
                       REQUIRES)

target_compile_options(${COMPONENT_LIB} 
						PRIVATE -std=gnu++17 
						-DCOAP_TE_ESP_IDF_PLATAFORM=1 
						-DCOAP_TE_PORT_C_STANDARD=1
						-DCOAP_TE_PORT_POSIX=1
						-DCOAP_TE_USE_SELECT=1
						-DCOAP_TE_USE_ERROR_MESSAGES=${USE_ERROR_MESSAGE}
						PUBLIC
						-DCOAP_TE_LOG_COLOR=${USE_LOG_COLOR}
						-DCOAP_TE_LOG_LEVEL=${USE_LOG_LEVEL}
						)