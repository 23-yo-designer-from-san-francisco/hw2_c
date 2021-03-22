cmake_minimum_required(VERSION 3.17)
project(hw2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS -lpthread)

include_directories(include)
link_directories(include)

#add_compile_options(-Wall -Wextra -pedantic -Werror)

add_executable(hw2 main.c src/multi.c src/single.c)