cmake_minimum_required(VERSION 3.25)
project(ctprintf LANGUAGES CXX)

include(GNUInstallDirs)

option(ENABLE_TESTING "Build and enable tests" OFF)

add_library(ctprintf INTERFACE)

add_library(ctprintf::ctprintf ALIAS ctprintf)

target_sources(
    ctprintf
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ctprintf/format.hpp>
              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ctprintf/output.hpp>
              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ctprintf/types.hpp>
              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ctprintf/detail/formatter.hpp>
              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ctprintf/detail/parser.hpp>)

target_include_directories(ctprintf INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                              $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(ctprintf INTERFACE cxx_std_20)

install(
    TARGETS ctprintf
    EXPORT ctprintf-targets
    INCLUDES
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    EXPORT ctprintf-targets
    NAMESPACE ctprintf::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctprintf)

install(FILES cmake/ctprintf-config.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctprintf)

if(ENABLE_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif() # ENABLE_TESTING
