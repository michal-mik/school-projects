cmake_minimum_required(VERSION 3.30)
project(zadanie1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

add_subdirectory(src)

install(TARGETS zadanie1)