cmake_minimum_required(VERSION 3.16)

project(s6502 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED true)

file(GLOB_RECURSE S6502_SRCS "src/*")
add_executable(s6502 ${S6502_SRCS})
target_include_directories(s6502
PUBLIC
    "src"
)