cmake_minimum_required(VERSION 3.25)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(DebianChangelog)
parse_debian_changelog("${CMAKE_SOURCE_DIR}/../debian/changelog")

project(
    ctprintf
    VERSION ${DEBIAN_PROJECT_VERSION}
    LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(ENABLE_TESTING "Build and enable tests" OFF)
option(ENABLE_BENCHMARKS "Build performance benchmarks" OFF)

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

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ctprintf-config-version.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMinorVersion)

install(
    FILES cmake/ctprintf-config.cmake "${CMAKE_CURRENT_BINARY_DIR}/ctprintf-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctprintf)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../README.md ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE
        DESTINATION ${CMAKE_INSTALL_DOCDIR})

if(ENABLE_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif() # ENABLE_TESTING

if(ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif() # ENABLE_BENCHMARKS

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_CONTACT "Thomas Willetal <t.willetal@googlemail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Compile-time checked printf-style formatting library")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CPACK_PACKAGE_VENDOR "embtom")
include(CPack)
