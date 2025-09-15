#pragma once
#include "s6502/bus.h"

// 6502 CPU state
typedef struct cpu_s cpu_t;

// 6502 CPU status bitflag indices
typedef enum {
    CPU_STATUS_NEGATIVE_INDEX = 0,
    CPU_STATUS_OVERFLOW_INDEX,
    CPU_STATUS_BREAK_INDEX,
    CPU_STATUS_DECIMAL_INDEX,
    CPU_STATUS_INTERRUPT_DISABLED_INDEX,
    CPU_STATUS_ZERO_INDEX,
    CPU_STATUS_CARRY_INDEX 
} cpu_status_indices;

// 6502 CPU status bitflags
typedef enum {
    CPU_STATUS_FLAG_NEGATIVE_BIT            = BIT(CPU_STATUS_NEGATIVE_INDEX),
    CPU_STATUS_FLAG_OVERFLOW_BIT            = BIT(CPU_STATUS_OVERFLOW_INDEX),
    CPU_STATUS_FLAG_BREAK_BIT               = BIT(CPU_STATUS_BREAK_INDEX),
    CPU_STATUS_FLAG_DECIMAL_BIT             = BIT(CPU_STATUS_DECIMAL_INDEX),
    CPU_STATUS_FLAG_INTERRUPT_DISABLED_BIT  = BIT(CPU_STATUS_INTERRUPT_DISABLED_INDEX),
    CPU_STATUS_FLAG_ZERO_BIT                = BIT(CPU_STATUS_ZERO_INDEX),
    CPU_STATUS_FLAG_CARRY_BIT               = BIT(CPU_STATUS_CARRY_INDEX)
} cpu_status_flags;

// 6502 CPU instruction opcodes
typedef enum {
    CPU_OPCODE_UNKNOWN = 0,
    CPU_OPCODE_ADC,         // Add with carry
    CPU_OPCODE_AND,         // And (with accumulator)
    CPU_OPCODE_ASL,         // Arithmetic shift left
    CPU_OPCODE_BCC,         // Branch on carry clear
    CPU_OPCODE_BCS,         // Branch on carry set
    CPU_OPCODE_BEQ,         // Branch on equal (zero set)
    CPU_OPCODE_BIT,         // Bit test
    CPU_OPCODE_BMI,         // Branch on minus (negative set)
    CPU_OPCODE_BNE,         // Branch on not equal (zero clear)
    CPU_OPCODE_BPL,         // Branch on plus (negative clear)
    CPU_OPCODE_BRK,         // Force interrupt
    CPU_OPCODE_BVC,         // Branch on overflow clear
    CPU_OPCODE_BVS,         // Branch on overflow set
    CPU_OPCODE_CLC,         // Clear carry flag
    CPU_OPCODE_CLD,         // Clear decimal mode
    CPU_OPCODE_CLI,         // Clear interrupt disable
    CPU_OPCODE_CLV,         // Clear overflow flag
    CPU_OPCODE_CMP,         // Compare (with accumulator)
    CPU_OPCODE_CPX,         // Compare with X
    CPU_OPCODE_CPY,         // Compare with Y
    CPU_OPCODE_DEC,         // Decrement memory
    CPU_OPCODE_DEX,         // Decrement X
    CPU_OPCODE_DEY,         // Decrement Y
    CPU_OPCODE_EOR,         // Exclusive OR (with accumulator)
    CPU_OPCODE_INC,         // Increment memory
    CPU_OPCODE_INX,         // Increment X
    CPU_OPCODE_INY,         // Increment Y
    CPU_OPCODE_JMP,         // Jump
    CPU_OPCODE_JSR,         // Jump to subroutine
    CPU_OPCODE_LDA,         // Load accumulator
    CPU_OPCODE_LDX,         // Load X
    CPU_OPCODE_LDY,         // Load Y
    CPU_OPCODE_LSR,         // Logical shift right
    CPU_OPCODE_NOP,         // No operation
    CPU_OPCODE_ORA,         // OR with accumulator
    CPU_OPCODE_PHA,         // Push accumulator
    CPU_OPCODE_PHP,         // Push processor status
    CPU_OPCODE_PLA,         // Pull accumulator
    CPU_OPCODE_PLP,         // Pull processor status
    CPU_OPCODE_ROL,         // Rotate left
    CPU_OPCODE_ROR,         // Rotate right
    CPU_OPCODE_RTI,         // Return from interrupt
    CPU_OPCODE_RTS,         // Return from subroutine
    CPU_OPCODE_SBC,         // Subtract with carry
    CPU_OPCODE_SEC,         // Set carry flag
    CPU_OPCODE_SED,         // Set decimal flag
    CPU_OPCODE_SEI,         // Set interrupt disable
    CPU_OPCODE_STA,         // Store accumulator
    CPU_OPCODE_STX,         // Store X
    CPU_OPCODE_STY,         // Store Y
    CPU_OPCODE_TAX,         // Transfer accumulator to X
    CPU_OPCODE_TAY,         // Transfer accumulator to Y
    CPU_OPCODE_TSX,         // Transfer stack pointer to X
    CPU_OPCODE_TXA,         // Transfer X to accumulator
    CPU_OPCODE_TXS,         // Transfer X to stack pointer
    CPU_OPCODE_TYA,         // Transfer Y to accumulator
    CPU_OPCODE_ENUM_MAX = U8_MAX
} cpu_opcode;


// 6502 CPU instruction address modes
typedef enum {
    CPU_ADDRESS_MODE_UNKNOWN = 0,
    CPU_ADDRESS_MODE_ACCUMULATOR,
    CPU_ADDRESS_MODE_ABSOLUTE,
    CPU_ADDRESS_MODE_ABSOLUTE_X,
    CPU_ADDRESS_MODE_ABSOLUTE_Y,
    CPU_ADDRESS_MODE_IMMEDIATE,
    CPU_ADDRESS_MODE_ZEROPAGE,
    CPU_ADDRESS_MODE_ZEROPAGE_X,
    CPU_ADDRESS_MODE_ZEROPAGE_Y,
    CPU_ADDRESS_MODE_INDIRECT,
    CPU_ADDRESS_MODE_INDIRECT_X,
    CPU_ADDRESS_MODE_INDIRECT_Y,
    CPU_ADDRESS_MODE_IMPLIED,
    CPU_ADDRESS_MODE_RELATIVE,
    CPU_ADDRESS_MODE_ENUM_MAX = U8_MAX
} cpu_address_mode;

typedef void (*cpu_instruction_fn)(cpu_t*, cpu_address_mode, u16);

typedef struct cpu_instruction_info_s {
    cpu_opcode opcode;
    cpu_address_mode address_mode;
    u8 size;
} cpu_instruction_info_t;

typedef struct cpu_instruction_s {
    cpu_instruction_info_t info;
    u16 operand;
} cpu_instruction_t;


// Create a 6502 CPU instance
// @param[in] addr_bus (optional) A memory block to be used as the CPU address bus
// @returns 6502 CPU instance pointer
cpu_t* cpu_create(bus_t* bus);

// Free a 6502 CPU instance
// @param[in] cpu The CPU instance to destroy
void cpu_free(cpu_t* cpu);

// Decode 4 byte chunk into 6502 CPU instruction
// @param[in] cpu
// @param[in] word Chunk to decode
// @returns Decoded instruction
cpu_instruction_t cpu_decode(cpu_t* cpu, u32 word);

// Executes a decoded CPU instruction using the given 6502 CPU instance
// @param[in] cpu
// @param[in] inst
void cpu_exec(cpu_t* cpu, cpu_instruction_t inst);

void cpu_push(cpu_t* cpu, u8 value);

u8 cpu_pop(cpu_t* cpu);

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