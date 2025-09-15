#include "s6502/bus.h"
#include "s6502/lib/interval_tree.h"

#define BUS_PCI_NODE_CACHE_SIZE 2

// The `bus` is essentially an interval tree structure that tracks all "attached" PCI units, 
// and maps memory reads/writes to the appropriate unit (invoking its respective function pointer).
struct bus_s {
    interval_node_t* pci_root;
    interval_node_t* pci_node_cache[BUS_PCI_NODE_CACHE_SIZE];
    u32 num_pci;
};

// Try to save a tree search by using the cached PCI nodes
static inline interval_node_t* bus_cache_search(bus_t* bus, u16 addr) {
    interval_node_t* node = NULL;

    for (u32 i = 0; i < BUS_PCI_NODE_CACHE_SIZE; i++) {
        node = bus->pci_node_cache[i];
        if (node && interval_node_test(node, addr))
            break;
    }

    return node;
}

static inline void bus_cache_push(bus_t* bus, interval_node_t* node) {
    // Push all elements down
    for (u32 i = BUS_PCI_NODE_CACHE_SIZE - 1; i > 0; i--) {
        bus->pci_node_cache[i] = bus->pci_node_cache[i - 1];
    }

    // Insert new first element
    bus->pci_node_cache[0] = node;
}


bus_t* bus_create() {
    return (bus_t*)calloc(1, sizeof(bus_t));
}

void bus_free(bus_t* bus) {
    free(bus);
}

b8 bus_attach_pci(bus_t* bus, pci_t* pci, u16 addr_start, u16 addr_end) {
    interval_node_t* pci_node = interval_tree_insert(bus->pci_root, addr_start, addr_end, (void*)pci);
    if (pci_node) {
        if (bus->pci_root == NULL)
            bus->pci_root = pci_node;

        bus->num_pci++;
        return TRUE;
    }

    return FALSE;
}

b8 bus_load(bus_t* bus, u16 addr, u8* load) {
    assert(bus->pci_root != NULL);

    interval_node_t* pci_node = bus_cache_search(bus, addr);
    if (!pci_node)
        pci_node = interval_tree_search(bus->pci_root, addr);
    
    if (pci_node) {
        pci_t* pci = interval_node_get_data(pci_node);
        if (pci && pci->on_load) {
            *load = pci->on_load(pci, addr);
            bus_cache_push(bus, pci_node);
            return TRUE;
        }
    }

    // TODO: should probably throw an exception instead
    *load = U8_MAX;
    return FALSE;
}

b8 bus_store(bus_t* bus, u16 addr, u8 value) {
    assert(bus->pci_root != NULL);

    interval_node_t* pci_node = bus_cache_search(bus, addr);
    if (!pci_node)
        pci_node = interval_tree_search(bus->pci_root, addr);

    if (pci_node) {
        pci_t* pci = interval_node_get_data(pci_node);
        if (pci && pci->on_store) {
            pci->on_store(pci, addr, value);
            bus_cache_push(bus, pci_node);
        }
    }

    // TODO: should probably throw an exception here too
    return FALSE;
}