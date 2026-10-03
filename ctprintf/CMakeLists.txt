cmake_minimum_required(VERSION 3.25)
project(ctprintf LANGUAGES CXX)

option(ENABLE_TESTING "Build and enable tests" OFF)

add_library(ctprintf INTERFACE)

add_library(ctprintf::ctprintf ALIAS ctprintf)

target_sources(
    ctprintf
    INTERFACE include/ctprintf/format.hpp
              include/ctprintf/output.hpp
              include/ctprintf/types.hpp
              include/ctprintf/detail/formatter.hpp
              include/ctprintf/detail/parser.hpp)

target_include_directories(ctprintf INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(ctprintf INTERFACE cxx_std_20)

if(ENABLE_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif() # ENABLE_TESTING
