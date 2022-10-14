#################################################
# Checking log variables						#
#################################################

set(USE_LOG_COLOR 	1)
if(DEFINED LOG_COLOR)
	set(USE_LOG_COLOR	${LOG_COLOR})
endif()

set(USE_LOG_LEVEL	5)
if(DEFINED LOG_LEVEL)
	set(USE_LOG_LEVEL	${LOG_LEVEL})
endif()

set(USE_ERROR_MESSAGE	1)
if(DEFINED ERROR_MESSAGE)
	set(USE_ERROR_MESSAGE ${ERROR_MESSAGE})
endif()

set(JSON_SUPPORT		FALSE)
if(DEFINED 	RAPIDJSON_PATH)
	set(RAPIDJSON_ROOT	${RAPIDJSON_PATH})
	set(JSON_SUPPORT	TRUE)
endif()

if(DEFINED COAP_TE_JSON_SUPPORT AND COAP_TE_JSON_SUPPORT)
	set(JSON_SUPPORT	TRUE)
endif()

#################################################
# Function helper								#
#################################################
function(define_if_not target ddefines ddefine dvalue)
	foreach(mydefines ${ddefines})
		if(${mydefines} MATCHES "^${ddefine}=")
			return()
		endif()
	endforeach()
	target_compile_definitions(${target} PUBLIC "${ddefine}=${dvalue}") 
endfunction()

#################################################
# Checking if is ESP IDF build					#
#################################################

if(ESP_PLATFORM EQUAL 1)
	include (${COMPONENT_DIR}/esp_idf.cmake)
	return()
endif()

#################################################
# Linux/Windows build
#################################################
cmake_minimum_required (VERSION 3.10)
project (coap-te LANGUAGES CXX VERSION 0.1)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

if(MSVC)
	set(CMAKE_CXX_FLAGS " /GR- /W4")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -fno-exceptions -fno-rtti") 
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

################################
# Including tree trunks
################################
find_package(tree_trunks REQUIRED)

include (files.cmake)

list(APPEND MAIN_SRC ${SRC_DIR_PORT}/c_standard_port.cpp)

#If Windows or WebAssembly (emscripten) use SELECT implementation
if(WIN32 OR MINGW OR EMSCRIPTEN)
	message("Setting SELECT call implmenetation")
	add_definitions(-DCOAP_TE_USE_SELECT=1)	
endif()

if(JSON_SUPPORT)
	message("Added JSON support")
	include_directories(${RAPIDJSON_ROOT})
	list(APPEND MAIN_SRC ${SRC_JSON_SUPPORT})
	add_definitions(-DCOAP_TE_JSON_HELPER=1)
endif()
				
add_library(${PROJECT_NAME} ${MAIN_SRC} ${SRC_PORT_POSIX})
add_library(${PRJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(${PROJECT_NAME} 
							PUBLIC tree_trunks::tree_trunks)

if (MINGW)
	target_link_libraries(${PROJECT_NAME} PUBLIC wsock32 ws2_32)
endif()

#Includes
target_include_directories(${PROJECT_NAME} 
							INTERFACE 	src)

#Definitions
target_compile_definitions(${PROJECT_NAME} 
							PUBLIC COAP_TE_PORT_C_STANDARD=1
							PUBLIC COAP_TE_PORT_POSIX=1)
#Define only if not defined previosly
get_directory_property(COAP_DEFINES COMPILE_DEFINITIONS)
define_if_not(${PROJECT_NAME} "${COAP_DEFINES}" "COAP_TE_LOG_COLOR" ${USE_LOG_COLOR})
define_if_not(${PROJECT_NAME} "${COAP_DEFINES}" "COAP_TE_LOG_LEVEL" ${USE_LOG_LEVEL})
define_if_not(${PROJECT_NAME} "${COAP_DEFINES}" "COAP_TE_USE_ERROR_MESSAGES" ${USE_ERROR_MESSAGE})
							
#Checking if must compile examples
if(DEFINED WITH_EXAMPLES)
	if(${WITH_EXAMPLES} EQUAL 1)
		include (examples/compile_example.cmake)
	endif()
endif()

########################################
# Installing
########################################
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY "src/"
        		DESTINATION "include"
        		FILES_MATCHING 
        		PATTERN "*.hpp"
        		PATTERN "*.h")
if(DEFINED WITH_EXAMPLES)
	if(${WITH_EXAMPLES} EQUAL 1)
		install(TARGETS coap_client DESTINATION bin)
	endif()
endif()
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md" 
				"${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
				"${CMAKE_CURRENT_SOURCE_DIR}/URL.txt"
			DESTINATION doc)

#########################################
# Packing // https://cmake.org/cmake/help/latest/module/CPack.html
#########################################
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VENDOR		"xeds corp")
set(CPACK_PACKAGE_DESCRIPTION	"C/C++ CoAP RFC7252 implementation")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C/C++ CoAP RFC7252 implementation")
set(CPACK_PACKAGE_HOMEPAGE_URL 	"https://github.com/rnascunha/coap-te")
set(CPACK_RESOURCE_FILE_README 	"${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include(CPack)