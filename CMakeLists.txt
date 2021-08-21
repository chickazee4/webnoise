cmake_minimum_required(VERSION 3.10)
project(webnoise)

find_library(
    LIBOPING
    NAMES liboping oping
)

add_executable(webnoise webnoise.c)
target_link_libraries(webnoise ${LIBOPING})
install(TARGETS webnoise)