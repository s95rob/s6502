# s6502-core

file(GLOB_RECURSE S6502_CORE_SRCS "src/*")
add_library(s6502-core ${S6502_CORE_SRCS})
target_include_directories(s6502-core
PUBLIC
    "include"
)