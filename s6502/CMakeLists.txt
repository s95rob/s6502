# s6502 (Executable)

file(GLOB_RECURSE S6502_SRCS "src/*")
add_executable(s6502 ${S6502_SRCS})
target_link_libraries(s6502
PRIVATE
    s6502-core
)