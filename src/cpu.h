#pragma once
#include "cpu_instruction.h"
#include "bus.h"

// 6502 CPU status bitflags
typedef enum {
    CPU_STATUS_FLAG_NEGATIVE_BIT            = BIT(0),
    CPU_STATUS_FLAG_OVERFLOW_BIT            = BIT(1),
    CPU_STATUS_FLAG_BREAK_BIT               = BIT(2),
    CPU_STATUS_FLAG_DECIMAL_BIT             = BIT(3),
    CPU_STATUS_FLAG_INTERRUPT_DISABLED_BIT  = BIT(4),
    CPU_STATUS_FLAG_ZERO_BIT                = BIT(5),
    CPU_STATUS_FLAG_CARRY_BIT               = BIT(6)
} cpu_status_flags;

// 6502 CPU state
typedef struct cpu_s cpu_t;


// Create a 6502 CPU instance
// @param[in] addr_bus (optional) A memory block to be used as the CPU address bus
// @returns 6502 CPU instance pointer
cpu_t* cpu_create(bus_t* bus);

// Free a 6502 CPU instance
// @param[in] cpu The CPU instance to destroy
void cpu_free(cpu_t* cpu);

void cpu_exec(cpu_t* cpu, const instruction_t* inst);

// Get the state of the 6502 CPU instance
// @param[in] cpu The CPU instance to obtain state from
// @param[out] a (optional) Accumulator register
// @param[out] x (optional) Index X register
// @param[out] y (optional) Index Y register
// @param[out] sp (optional) Stack pointer register
// @param[out] status (optional) Status register 
// @param[out] pc (optional) Program counter register
// @param[out] cycles (optional) Current cycle count
void cpu_get_state(cpu_t* cpu, u8* a, u8* x, u8* y, u8* sp, u8* status, u16* pc, u64* cycles);