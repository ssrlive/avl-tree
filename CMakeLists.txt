cmake_minimum_required(VERSION 3.0)
project(bst-tree)

if (UNIX)
add_definitions(-Wall -Werror -ggdb3 -std=c90 -Wextra -pedantic)
add_definitions("-D_DEFAULT_SOURCE -D_GNU_SOURCE")
endif()

add_executable(bst-tree
    bst-tree.c bst-tree.h test.c)
