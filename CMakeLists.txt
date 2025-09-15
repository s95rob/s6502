# s6502 (Project)

cmake_minimum_required(VERSION 3.16)

project(s6502 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED true)

add_subdirectory("s6502-core")
add_subdirectory("s6502")