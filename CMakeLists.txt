cmake_minimum_required(VERSION 3.10)
PROJECT(coap-te LANGUAGES CXX VERSION 0.1.0)

option(COAP_TE_MESSAGE_OPTION_OBSERVABLE_RESOURCE "make resource observable" ON)
option(COAP_TE_MESSAGE_OPTION_HOP_LIMIT "add hop limit option" ON)
option(COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER "add blockwise transfer" ON)
option(COAP_TE_MESSAGE_OPTION_NO_RESPONSE "add no_response option" ON)

add_compile_definitions(COAP_TE_MESSAGE_OPTION_OBSERVABLE_RESOURCE=$<BOOL:${COAP_TE_MESSAGE_OPTION_OBSERVABLE_RESOURCE}>
				COAP_TE_MESSAGE_OPTION_HOP_LIMIT=$<BOOL:${COAP_TE_MESSAGE_OPTION_HOP_LIMIT}>
				COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER=$<BOOL:${COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER}>
				COAP_TE_MESSAGE_OPTION_NO_RESPONSE=$<BOOL:${COAP_TE_MESSAGE_OPTION_NO_RESPONSE}>)

if(MSVC)
	set(CMAKE_CXX_FLAGS " /GR- /W4")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -fno-exceptions -fno-rtti") 
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

include(examples/examples.cmake)
include(tests/tests.cmake)