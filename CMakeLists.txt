cmake_minimum_required(VERSION 3.8)
project(n_ary_tree_huffman)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
        main.cpp
        )
add_executable(n_ary_tree_huffman ${SOURCE_FILES})

set(OPTS -O3 -Werror -Wall -Wextra)

target_compile_options(n_ary_tree_huffman PUBLIC ${OPTS})