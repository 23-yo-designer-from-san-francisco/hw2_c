cmake_minimum_required(VERSION 3.16.3)
project(hw2 C CXX)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER "gcc-10")
set(CMAKE_CXX_COMPILER "g++-10")

set(CMAKE_C_FLAGS "-O0")


add_compile_options(-Wall -Wpedantic -Werror -Wno-vla --coverage)

enable_testing()

find_package(Threads REQUIRED)

include(ExternalProject)

ExternalProject_Add(
        gtest
        URL https://github.com/google/googletest/archive/release-1.10.0.zip
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
        INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gtest source_dir binary_dir)

add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

set_target_properties(libgtest PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/lib/libgtest.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)

set_target_properties(libgmock PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/lib/libgmock.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

include_directories(
        "${source_dir}/googletest/include"
        "${source_dir}/googlemock/include"

        "${PROJECT_SOURCE_DIR}/lib/include"
        "${PROJECT_SOURCE_DIR}/lib/src"
)

add_test(
        NAME test_single
        COMMAND test_single
)

add_test(
        NAME test_multi
        COMMAND test_multi
)

add_executable(test_single test/test_single.cpp)
add_executable(test_multi test/test_multi.cpp)

target_link_libraries(
        test_single
        libgtest
        libgmock
        gcov
)
target_link_libraries(
        test_multi
        libgtest
        libgmock
        pthread
        gcov
)

add_executable(hw2 main.c)

find_package(single CONFIG REQUIRED)
target_link_libraries(hw2 Single::single -fprofile-arcs dl pthread)


