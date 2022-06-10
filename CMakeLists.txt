cmake_minimum_required(VERSION 3.0)
project(avl-tree)

if (UNIX)
add_definitions(-Wall -Werror -ggdb3 -std=c90 -Wextra -pedantic)
add_definitions("-D_DEFAULT_SOURCE -D_GNU_SOURCE")
endif()

if (MSVC)
add_compile_options(/wd4996)
endif()

add_executable(avl-tree
    avl-tree.c avl-tree.h test.c)
