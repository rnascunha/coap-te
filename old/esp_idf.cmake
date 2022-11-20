include (${COMPONENT_DIR}/files.cmake)

list(APPEND MAIN_SRC ${SRC_DIR_PORT}/c_standard_port.cpp)

idf_component_register(SRCS ${MAIN_SRC} ${SRC_PORT_POSIX} ${SRC_PORT_ESP_MESH}
                       INCLUDE_DIRS ${MAIN_INCLUDE_DIRS}
                       REQUIRES esp_timer)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/third/tree_trunks")
target_link_libraries(${COMPONENT_LIB} PUBLIC tree_trunks)

if(CONFIG_COAP_TE_MESSAGE_ERROR)
	set(CONFIG_COAP_TE_MESSAGE_ERROR 1)
else()
	set(CONFIG_COAP_TE_MESSAGE_ERROR 0)
endif()

if(CONFIG_COAP_TE_USE_COLOR)
	set(CONFIG_COAP_TE_USE_COLOR 1)
else()
	set(CONFIG_COAP_TE_USE_COLOR 0)
endif()

if(NOT CONFIG_COAP_TE_USE_COLOR)
	set(CONFIG_COAP_TE_USE_COLOR 5)
endif()

target_compile_options(${COMPONENT_LIB} 
						PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17> 
							-DCOAP_TE_ESP_IDF_PLATAFORM=1 
							-DCOAP_TE_PORT_C_STANDARD=1
							-DCOAP_TE_PORT_POSIX=1
							-DCOAP_TE_PORT_ESP_MESH=1
							-DCOAP_TE_USE_SELECT=1
							-DCOAP_TE_USE_ERROR_MESSAGES=${CONFIG_COAP_TE_MESSAGE_ERROR}
							-DCOAP_TE_LOG_COLOR=${CONFIG_COAP_TE_USE_COLOR}
							-DCOAP_TE_LOG_LEVEL=${CONFIG_COAP_TE_LOG_LEVEL})