#include "cpu.h"
#include "pci.h"

#include <stdio.h>

void on_attach(pci_t* pci) {
    printf("PCI attached: %s\n", pci->name);
}

u8 on_load(pci_t* pci, u16 addr) {
    printf("PCI on_load: %s %i\n", pci->name, addr);
    return 1;
}

void on_store(pci_t* pci, u16 addr, u8 value) {
    printf("PCI on_store: %s %i %i\n", pci->name, addr, value);
}

int main(int argc, char** argv) {
    bus_t* bus = bus_create();
    cpu_t* cpu = cpu_create(bus);

    u32 word = 0xa9ff0000; // LDA #255
    cpu_instruction_t inst = cpu_decode(cpu, word);
    cpu_exec(cpu, inst);

    pci_t pci = {
        .name = "TestPCI",
        .on_attach = on_attach,
        .on_load = on_load,
        .on_store = on_store
    };

    pci.on_attach(&pci);
    pci.on_load(&pci, 0x1000);
    pci.on_store(&pci, 0x1000, 1);

    cpu_free(cpu);
    bus_free(bus);

    return 0;
}