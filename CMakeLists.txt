cmake_minimum_required(VERSION 3.17)
project(hw2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS -lpthread)

include_directories(include)

add_library(single STATIC include/single.h src/single.c)
install(TARGETS single EXPORT singleTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
)
install(FILES include/single.h DESTINATION include)
install(EXPORT singleTargets
        FILE singleConfig.cmake
        NAMESPACE Single::
        DESTINATION lib/cmake/Single)

add_library(multi SHARED include/multi.h src/multi.c)
install(TARGETS multi EXPORT multiTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
        )
install(FILES include/single.h DESTINATION include)
install(EXPORT multiTargets
        FILE multiConfig.cmake
        NAMESPACE Multi::
        DESTINATION lib/cmake/Multi)

#add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(hw2_single main.c)
add_executable(hw2_multi main.c)

find_package(single CONFIG REQUIRED)
target_link_libraries(hw2_single Single::single)

find_package(multi CONFIG REQUIRED)
target_link_libraries(hw2_multi Multi::multi)