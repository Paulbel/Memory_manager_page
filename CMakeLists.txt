cmake_minimum_required(VERSION 3.8)
project(memory_manager)

set(CMAKE_C_STANDARD 99)
set(SOURCE_FILES main.c  paging.c tests/load_tests.c tests/unit_tests.c)
add_executable(memory_manager ${SOURCE_FILES})