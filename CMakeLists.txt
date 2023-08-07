cmake_minimum_required(VERSION 3.8)

project(gtav)

add_compile_options(-Wall -Wextra -Wpedantic -Wformat -Wunused -Wcast-qual -Wcast-align=strict -Wduplicated-branches -Wduplicated-cond -Wnull-dereference -Wno-psabi -Wlogical-op)
#add_compile_options(-Wswitch-enum)
#add_compile_options(-Wconversion)
#add_compile_options(-Wshadow)

set (CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)
