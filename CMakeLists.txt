cmake_minimum_required(VERSION 3.13)
project(jcast)
set(CMAKE_CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

find_package(raylib 5.0 REQUIRED) # Requires at least version 3.0

add_compile_options(
-Wall
-pedantic
)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(examples)
