cmake_minimum_required (VERSION 3.10)
project (coap-te)

message(STATUS "Building OS: " ${CMAKE_SYSTEM_NAME})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

message("Builder type: " ${CMAKE_BUILD_TYPE}) 

if(MSVC)
	message(STATUS "MSVC build")
	set(CMAKE_CXX_FLAGS "/W4 /std:c++17 /EHsc /bigobj")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
	add_compile_definitions(BOOST_DATE_TIME_NO_LIB BOOST_REGEX_NO_LIB)
else()
	message(STATUS "NO MSVC build")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter") 
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

if(WIN32)
    macro(get_WIN32_WINNT version)
        if(CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
            string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
            # Check for Windows 10, b/c we'll need to convert to hex 'A'.
            if("${verMajor}" MATCHES "10")
                set(verMajor "A")
                string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
            endif()
            # Remove all remaining '.' characters.
            string(REPLACE "." "" ver ${ver})
            # Prepend each digit with a zero.
            string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
            set(${version} "0x${ver}")
        endif()
    endmacro()

    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
        message("WIN32: " ${ver})
endif()

message(STATUS "Compiler: " ${CMAKE_CXX_COMPILER_ID} " " ${CMAKE_CXX_COMPILER_VERSION})

set(SRC_DIR ./src)
set(SRC_DIR_MESSAGE ${SRC_DIR}/message)
set(SRC_DIR_TRANSMISSION ${SRC_DIR}/transmission)
set(SRC_DIR_DEBUG ${SRC_DIR}/debug)
set(SRC_DIR_INTERNAL ${SRC_DIR}/internal)

set(MAIN_SRC ${SRC_DIR}/main.cpp 
				${SRC_DIR}/error.cpp
				${SRC_DIR}/uri/compose.cpp
				${SRC_DIR}/uri/decompose.cpp
				${SRC_DIR_INTERNAL}/helper.cpp
				${SRC_DIR_MESSAGE}/options.cpp
				${SRC_DIR_MESSAGE}/message_id.cpp
				${SRC_DIR_MESSAGE}/types.cpp
				${SRC_DIR_MESSAGE}/codes.cpp
				${SRC_DIR_MESSAGE}/parser.cpp
				${SRC_DIR_MESSAGE}/serialize.cpp
				${SRC_DIR_TRANSMISSION}/functions.cpp
				${SRC_DIR_DEBUG}/helper.cpp
				${SRC_DIR_DEBUG}/print_options.cpp
				${SRC_DIR_DEBUG}/output_string.cpp
				${SRC_DIR_DEBUG}/print_message.cpp
				${SRC_DIR_DEBUG}/print_uri.cpp
				)

if(NOT WIN32)
	message(STATUS "Adding linux dependecies...")
#	list(APPEND MAIN_SRC ${SRC_DIR}/port/linux/socket.cpp)
else()
	message(FATAL_ERROR "No system support")
endif()

add_executable(${PROJECT_NAME} ${MAIN_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

include_directories(./src ./third/tree_trunks/src/include)

#if(WIN32)
#	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
#else()
#	target_link_libraries(${PROJECT_NAME} pthread)
#endif()
