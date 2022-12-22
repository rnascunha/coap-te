cmake_minimum_required(VERSION 3.10)
PROJECT(coap-te LANGUAGES CXX VERSION 0.1.0)

#####################
# Compile options	#
#####################
option(COAP_TE_WITH_EXAMPLES "compile examples"	ON)
option(COAP_TE_WITH_TESTS	 "compile tests"    	ON)
option(COAP_TE_WITH_DOCS	 "compile docs"		    OFF)

#
# Choose CoAP options to compile
#
option(COAP_TE_MESSAGE_OPTION_OBSERVABLE_RESOURCE 	"make resource observable"	ON)
option(COAP_TE_MESSAGE_OPTION_HOP_LIMIT 			      "add hop limit option" 		  ON)
option(COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER 	  "add blockwise transfer" 	  ON)
option(COAP_TE_MESSAGE_OPTION_NO_RESPONSE 			    "add no_response option" 	  ON)

option(COAP_TE_ENABLE_FETCH_PATCH_VERBS             "add fetch/patch verbs"     ON)
option(COAP_TE_ENABLE_STREAM_CONNECTION             "enable TCP/Websocket conn" ON)

option(COAP_TE_ENABLE_EXCEPTIONS                    "enable exceptions"         OFF)
option(COAP_TE_ERROR_SHOW_MESSAGE                   "add error messages"        ON)
option(COAP_TE_ENABLE_STD_ERROR_CODE                "enable std::error_code"    ON)

add_compile_definitions(
        COAP_TE_MESSAGE_OPTION_OBSERVABLE_RESOURCE=$<BOOL:${COAP_TE_MESSAGE_OPTION_OBSERVABLE_RESOURCE}>
				COAP_TE_MESSAGE_OPTION_HOP_LIMIT=$<BOOL:${COAP_TE_MESSAGE_OPTION_HOP_LIMIT}>
				COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER=$<BOOL:${COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER}>
				COAP_TE_MESSAGE_OPTION_NO_RESPONSE=$<BOOL:${COAP_TE_MESSAGE_OPTION_NO_RESPONSE}>
        COAP_TE_ENABLE_FETCH_PATCH_VERBS=$<BOOL:${COAP_TE_ENABLE_FETCH_PATCH_VERBS}>
        COAP_TE_ENABLE_STREAM_CONNECTION=$<BOOL:${COAP_TE_ENABLE_STREAM_CONNECTION}>
        COAP_TE_ERROR_SHOW_MESSAGE=$<BOOL:${COAP_TE_ERROR_SHOW_MESSAGE}>
        COAP_TE_ENABLE_STD_ERROR_CODE=$<BOOL:${COAP_TE_ENABLE_STD_ERROR_CODE}>
        COAP_TE_ENABLE_EXCEPTIONS=$<BOOL:${COAP_TE_ENABLE_EXCEPTIONS}>)

if(MSVC)
	set(CMAKE_CXX_FLAGS "/GR- /W4 /EHsc")
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

# Compile examples
if (COAP_TE_WITH_EXAMPLES)
	include(examples/examples.cmake)
endif()

# Compile and run tests
if (COAP_TE_WITH_TESTS)
	include(tests/tests.cmake)
endif()

# Compile documentation
if (COAP_TE_WITH_DOCS)
  include(docs/docs.cmake)
endif()