find_package(GTest CONFIG REQUIRED)
include(GoogleTest)
try_compile(
    CTPRINTF_ACCEPTS_INVALID_FORMAT ${CMAKE_CURRENT_BINARY_DIR}/compile_fail/type_mismatch
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_fail/type_mismatch.cpp
    CMAKE_FLAGS -DCMAKE_CXX_STANDARD=20
    OUTPUT_VARIABLE CTPRINTF_INVALID_FORMAT_OUTPUT)
if(CTPRINTF_ACCEPTS_INVALID_FORMAT)
    message(FATAL_ERROR "ctprintf accepted an argument with the wrong format type")
endif()

add_executable(ctprintf_tests format_integer.cpp format_string.cpp)

target_link_libraries(ctprintf_tests PRIVATE ctprintf::ctprintf GTest::gtest_main)

gtest_discover_tests(ctprintf_tests)
