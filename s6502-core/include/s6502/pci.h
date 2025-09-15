#pragma once
#include "s6502/common.h"

typedef struct pci_s pci_t;

typedef void (*pci_on_attach_fn)(pci_t*);
typedef u8 (*pci_on_load_fn)(pci_t*, u16);
typedef void (*pci_on_store_fn)(pci_t*, u16, u8);

struct pci_s {
    const char* name;
    void* data;
    pci_on_attach_fn on_attach;
    pci_on_load_fn on_load;
    pci_on_store_fn on_store;
};