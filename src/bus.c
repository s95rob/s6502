#include "bus.h"
#include "lib/interval_tree.h"

// The `bus` is essentially an interval tree structure that tracks all "attached" PCI units, 
// and maps memory reads/writes to the appropriate unit (invoking its respective function pointer).
struct bus_s {
    interval_node_t* pci_root;
    interval_node_t* pci_cached_node;
    u32 num_pci;
};

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

b8 bus_load(bus_t* bus, u16 addr, u8* value) {
    assert(bus->pci_root != NULL);

    interval_node_t* pci_node = NULL;

    // Try to save a search by using the cached node
    if (bus->pci_cached_node && interval_node_test(bus->pci_cached_node, addr))
        pci_node = bus->pci_cached_node;
    else
        pci_node = interval_tree_search(bus->pci_root, addr);

    if (pci_node) {
        pci_t* pci = interval_node_get_data(pci_node);

        if (pci) {
            *value = pci->on_load(pci, addr);
            bus->pci_cached_node = pci_node;
            return TRUE;
        }
    }

    // TODO: should probably throw an exception instead
    *value = U8_MAX;
    return FALSE;
}


b8 bus_store(bus_t* bus, u16 addr, u8 value) {
    assert(bus->pci_root != NULL);

    interval_node_t* pci_node = NULL;

    if (bus->pci_cached_node && interval_node_test(bus->pci_cached_node, addr))
        pci_node = bus->pci_cached_node;
    else
        pci_node = interval_tree_search(bus->pci_root, addr);

    if (pci_node) {
        pci_t* pci = interval_node_get_data(pci_node);

        if (pci) {
            pci->on_store(pci, addr, value);
            bus->pci_cached_node = pci_node;
            return TRUE;
        }
    }

    // TODO: should probably throw an exception here too
    return FALSE;
}