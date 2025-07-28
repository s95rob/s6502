#pragma once
#include "pci.h"

// Maximum length of addressable memory
#define BUS_ADDR_MAX 0xffff

// 6502 Address Bus
typedef struct bus_s bus_t;

// @returns New address bus instance
bus_t* bus_create();

// Frees the address bus
// @param[in] bus The address bus to destroy
void bus_free(bus_t* bus);

// Attaches a PCI unit to the address bus. Attached PCI units must have discrete address mappings. 
// @param[in] bus The address bus to attach the PCI to
// @param[in] pci The PCI unit to attach
// @param[in] addr_start The PCI unit's start address
// @param[in] addr_end The PCI unit's end address
// @returns True on success, false on failure (address range overlap)
b8 bus_attach_pci(bus_t* bus, pci_t* pci, u16 addr_start, u16 addr_end);

// Attempts to load an 8-bit unsigned value from the address bus
// @param[in] bus Address bus instance
// @param[in] addr Where to load the value from on the bus
// @param[out] load Where to load the value
// @returns True on success, false on failure 
b8 bus_load(bus_t* bus, u16 addr, u8* load);

// Attempts to store an 8-bit unsigned value to the address bus
// @param[in] bus Address bus instance
// @param[in] addr Where to store the value on the bus
// @param[in] value The value to store
// @returns True on success, false on failure
b8 bus_store(bus_t* bus, u16 addr, u8 value);