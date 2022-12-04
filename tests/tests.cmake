# From: https://google.github.io/googletest/quickstart-cmake.html
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    # DOWNLOAD_EXTRACT_TIMESTAMP true
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(TEST_DIR    tests)
set(TESTS       utility 
                sorted_no_alloc_list
                core_traits
                options_definitions
                buffer
                byte_order
                option_serialize_parse)
set(OUTPUT_TEST    ${CMAKE_BINARY_DIR}/tests)

include(GoogleTest)
foreach(TEST ${TESTS})
    add_executable(
        ${TEST}
        ${TEST_DIR}/${TEST}.cpp)
    
    target_link_libraries(
        ${TEST}
        GTest::gtest_main
        gmock
        ${PROJECT_NAME})

    set_target_properties(${TEST} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_TEST})

    gtest_discover_tests(${TEST})
endforeach()