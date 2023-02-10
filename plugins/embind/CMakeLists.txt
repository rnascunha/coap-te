#
# Calling:
#   cmake -DCMAKE_TOOLCHAIN_FILE=${EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake ..
#
# EMSDK must be defined
#
cmake_minimum_required(VERSION 3.10)
PROJECT(emcoap-te LANGUAGES CXX VERSION 0.1.0)

####################
# Checking EMSDK. EMSDK must be set
####################
if (NOT DEFINED ENV{EMSDK})
  message(FATAL_ERROR "Emscripten path not defined")
endif()

set(EMSDK_ROOT $ENV{EMSDK})
set(EMSDK_PATH "${EMSDK_ROOT}/upstream/emscripten")

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH PROJECT_ROOT_PATH)
cmake_path(GET PROJECT_ROOT_PATH PARENT_PATH PROJECT_ROOT_PATH)

####################
# Checking input
####################
option(RUN_BROWSER  "run emrun browser" ON)
option(MODULARIZE  "modularize output" OFF)

if (${RUN_BROWSER} AND ${MODULARIZE})
  message(FATAL_ERROR "MODULARIZE and RUN_BROWSER can't be set at the same time")
endif()

if (DEFINED OUTPUT_SUFFIX)
  if (${OUTPUT_SUFFIX} STREQUAL "html" OR
      ${OUTPUT_SUFFIX} STREQUAL "js")
      set(OUTPUT_SUFFIX ".${OUTPUT_SUFFIX}")
  else()
    message(FATAL_ERROR "Suffix not supported [${OUTPUT_SUFFIX}]")
  endif()
else()
  set(OUTPUT_SUFFIX ".html")
endif()

if (${RUN_BROWSER})
  set(OUTPUT_SUFFIX ".html")
endif()

if (${MODULARIZE})
  set(OUTPUT_SUFFIX ".js")
endif()

###########################################

add_compile_definitions(
        COAP_TE_MESSAGE_OPTION_OBSERVABLE_RESOURCE=1
				COAP_TE_MESSAGE_OPTION_HOP_LIMIT=1
				COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER=1
				COAP_TE_MESSAGE_OPTION_NO_RESPONSE=1
        COAP_TE_ENABLE_FETCH_PATCH_VERBS=1
        COAP_TE_ENABLE_STREAM_CONNECTION=1
        COAP_TE_ERROR_SHOW_MESSAGE=1
        COAP_TE_ENABLE_STD_ERROR_CODE=1
        COAP_TE_ENABLE_EXCEPTIONS=1)

include_directories(${PROJECT_ROOT_PATH}/include)

set(SRC code.cpp debug.cpp)
# set(SRC code.cpp)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PUBLIC embind)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        # RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_TEST}
        SUFFIX  ${OUTPUT_SUFFIX})

if (${MODULARIZE})
  target_link_options(${PROJECT_NAME} PUBLIC "-sMODULARIZE")
endif()

if (RUN_BROWSER)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                     COMMAND ${EMSDK_PATH}/emrun "${PROJECT_NAME}.html"
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()