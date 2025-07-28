#include "cpu.h"

#include <stdio.h>

int main(int argc, char** argv) {
    bus_t* bus = bus_create();
    cpu_t* cpu = cpu_create(bus);

    u32 word = 0xa9ff0000; // LDA #255
    cpu_instruction_t inst = cpu_decode(cpu, word);
    cpu_exec(cpu, inst);

    cpu_free(cpu);
    bus_free(bus);

    return 0;
}