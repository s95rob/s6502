// 6502 Instruction Reference:
// https://www.masswerk.at/6502/6502_instruction_set.html

#pragma once
#include "common.h"

// 6502 CPU instruction opcodes
typedef enum {
    OPCODE_UNKNOWN = 0,
    OPCODE_ADC,         // Add with carry
    OPCODE_AND,         // And (with accumulator)
    OPCODE_ASL,         // Arithmetic shift left
    OPCODE_BCC,         // Branch on carry clear
    OPCODE_BCS,         // Branch on carry set
    OPCODE_BEQ,         // Branch on equal (zero set)
    OPCODE_BIT,         // Bit test
    OPCODE_BMI,         // Branch on minus (negative set)
    OPCODE_BNE,         // Branch on not equal (zero clear)
    OPCODE_BPL,         // Branch on plus (negative clear)
    OPCODE_BRK,         // Force interrupt
    OPCODE_BVC,         // Branch on overflow clear
    OPCODE_BVS,         // Branch on overflow set
    OPCODE_CLC,         // Clear carry flag
    OPCODE_CLD,         // Clear decimal mode
    OPCODE_CLI,         // Clear interrupt disable
    OPCODE_CLV,         // Clear overflow flag
    OPCODE_CMP,         // Compare (with accumulator)
    OPCODE_CPX,         // Compare with X
    OPCODE_CPY,         // Compare with Y
    OPCODE_DEC,         // Decrement memory
    OPCODE_DEX,         // Decrement X
    OPCODE_DEY,         // Decrement Y
    OPCODE_EOR,         // Exclusive OR (with accumulator)
    OPCODE_INC,         // Increment memory
    OPCODE_INX,         // Increment X
    OPCODE_INY,         // Increment Y
    OPCODE_JMP,         // Jump
    OPCODE_JSR,         // Jump to subroutine
    OPCODE_LDA,         // Load accumulator
    OPCODE_LDX,         // Load X
    OPCODE_LDY,         // Load Y
    OPCODE_LSR,         // Logical shift right
    OPCODE_NOP,         // No operation
    OPCODE_ORA,         // OR with accumulator
    OPCODE_PHA,         // Push accumulator
    OPCODE_PHP,         // Push processor status
    OPCODE_PLA,         // Pull accumulator
    OPCODE_PLP,         // Pull processor status
    OPCODE_ROL,         // Rotate left
    OPCODE_ROR,         // Rotate right
    OPCODE_RTI,         // Return from interrupt
    OPCODE_RTS,         // Return from subroutine
    OPCODE_SBC,         // Subtract with carry
    OPCODE_SEC,         // Set carry flag
    OPCODE_SED,         // Set decimal flag
    OPCODE_SEI,         // Set interrupt disable
    OPCODE_STA,         // Store accumulator
    OPCODE_STX,         // Store X
    OPCODE_STY,         // Store Y
    OPCODE_TAX,         // Transfer accumulator to X
    OPCODE_TAY,         // Transfer accumulator to Y
    OPCODE_TSX,         // Transfer stack pointer to X
    OPCODE_TXA,         // Transfer X to accumulator
    OPCODE_TXS,         // Transfer X to stack pointer
    OPCODE_TYA,         // Transfer Y to accumulator
    OPCODE_ENUM_MAX = U8_MAX
} opcode;


// 6502 CPU instruction address modes
typedef enum {
    ADDRESS_MODE_UNKNOWN = 0,
    ADDRESS_MODE_ABSOLUTE,
    ADDRESS_MODE_ABSOLUTE_X,
    ADDRESS_MODE_ABSOLUTE_Y,
    ADDRESS_MODE_IMMEDIATE,
    ADDRESS_MODE_ZEROPAGE,
    ADDRESS_MODE_ZEROPAGE_X,
    ADDRESS_MODE_ZEROPAGE_Y,
    ADDRESS_MODE_INDIRECT,
    ADDRESS_MODE_INDIRECT_X,
    ADDRESS_MODE_INDIRECT_Y,
    ADDRESS_MODE_IMPLIED,
    ADDRESS_MODE_RELATIVE,
    ADDRESS_MODE_ENUM_MAX = U8_MAX
} address_mode;


// 6502 CPU instruction info
typedef struct instruction_info_s {
    opcode opcode;
    address_mode address_mode;
    u8 size;
} instruction_info_t;


// 6502 CPU instruction containing instruction info and arguments
typedef struct instruction_s {
    instruction_info_t info;
    u16 operand;
} instruction_t;


static const instruction_info_t g_instruction_info_table[] = {
    { OPCODE_BRK, ADDRESS_MODE_IMPLIED, 1 },        // 0x00
    { OPCODE_ORA, ADDRESS_MODE_INDIRECT_X, 2 },     // 0x01
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x02
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x03
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x04
    { OPCODE_ORA, ADDRESS_MODE_ZEROPAGE, 2 },       // 0x05
    { OPCODE_ASL, ADDRESS_MODE_ZEROPAGE, 2 },       // 0x06
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x07
    { OPCODE_PHP, ADDRESS_MODE_IMPLIED, 1 },        // 0x08
    { OPCODE_ORA, ADDRESS_MODE_IMMEDIATE, 2 },      // 0x09
    { OPCODE_ASL, ADDRESS_MODE_IMPLIED, 1 },        // 0x0A
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x0B
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x0C
    { OPCODE_ORA, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x0D
    { OPCODE_ASL, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x0E
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x0F

    { OPCODE_BPL, ADDRESS_MODE_RELATIVE, 2 },       // 0x10
    { OPCODE_ORA, ADDRESS_MODE_INDIRECT_Y, 2 },     // 0x11
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x12
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x13
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x14
    { OPCODE_ORA, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x15
    { OPCODE_ASL, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x16
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x17
    { OPCODE_CLC, ADDRESS_MODE_IMPLIED, 1 },        // 0x18
    { OPCODE_ORA, ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0x19
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x1A
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x1B
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x1C
    { OPCODE_ORA, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x1D
    { OPCODE_ASL, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x1E
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x1F

    { OPCODE_JSR, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x20
    { OPCODE_AND, ADDRESS_MODE_INDIRECT_X, 2 },     // 0x21
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x22
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x23
    { OPCODE_BIT, ADDRESS_MODE_ZEROPAGE, 2 },       // 0x24
    { OPCODE_AND, ADDRESS_MODE_ZEROPAGE, 2 },       // 0x25
    { OPCODE_ROL, ADDRESS_MODE_ZEROPAGE, 2 },       // 0x26
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x27
    { OPCODE_PLP, ADDRESS_MODE_IMPLIED, 1 },        // 0x28
    { OPCODE_AND, ADDRESS_MODE_IMMEDIATE, 2 },      // 0x29
    { OPCODE_ROL, ADDRESS_MODE_IMPLIED, 1 },        // 0x2A
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x2B
    { OPCODE_BIT, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x2C
    { OPCODE_AND, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x2D
    { OPCODE_ROL, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x2E
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x2F

    { OPCODE_BMI, ADDRESS_MODE_RELATIVE, 2 },       // 0x30
    { OPCODE_AND, ADDRESS_MODE_INDIRECT_Y, 2 },     // 0x31
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x32
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x33
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x34
    { OPCODE_AND, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x35
    { OPCODE_ROL, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x36
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x37
    { OPCODE_SEC, ADDRESS_MODE_IMPLIED, 1 },        // 0x38
    { OPCODE_AND, ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0x39
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x3A
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x3B
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x3C
    { OPCODE_AND, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x3D
    { OPCODE_ROL, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x3E
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x3F

    { OPCODE_RTI, ADDRESS_MODE_IMPLIED, 1 },        // 0x40
    { OPCODE_EOR, ADDRESS_MODE_INDIRECT_X, 2 },     // 0x41
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x42
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x43
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x44
    { OPCODE_EOR, ADDRESS_MODE_ZEROPAGE, 2 },       // 0x45
    { OPCODE_LSR, ADDRESS_MODE_ZEROPAGE, 2 },       // 0x46
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x47
    { OPCODE_PHA, ADDRESS_MODE_IMPLIED, 1 },        // 0x48
    { OPCODE_EOR, ADDRESS_MODE_IMMEDIATE, 2 },      // 0x49
    { OPCODE_LSR, ADDRESS_MODE_IMPLIED, 1 },        // 0x4A
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x4B
    { OPCODE_JMP, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x4C
    { OPCODE_EOR, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x4D
    { OPCODE_LSR, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x4E
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x4F

    { OPCODE_BVC, ADDRESS_MODE_RELATIVE, 2 },       // 0x50
    { OPCODE_EOR, ADDRESS_MODE_INDIRECT_Y, 2 },     // 0x51
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x52
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x53
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x54
    { OPCODE_EOR, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x55
    { OPCODE_LSR, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x56
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x57
    { OPCODE_CLI, ADDRESS_MODE_IMPLIED, 1 },        // 0x58
    { OPCODE_EOR, ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0x59
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x5A
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x5B
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x5C
    { OPCODE_EOR, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x5D
    { OPCODE_LSR, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x5E
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x5F

    { OPCODE_RTS, ADDRESS_MODE_IMPLIED, 1 },        // 0x60
    { OPCODE_ADC, ADDRESS_MODE_INDIRECT_X, 2 },     // 0x61
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x62
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x63
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x64
    { OPCODE_ADC, ADDRESS_MODE_ZEROPAGE, 2 },       // 0x65
    { OPCODE_ROR, ADDRESS_MODE_ZEROPAGE, 2 },       // 0x66
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x67
    { OPCODE_PLA, ADDRESS_MODE_IMPLIED, 1 },        // 0x68
    { OPCODE_ADC, ADDRESS_MODE_IMMEDIATE, 2 },      // 0x69
    { OPCODE_ROR, ADDRESS_MODE_IMPLIED, 1 },        // 0x6A
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x6B
    { OPCODE_JMP, ADDRESS_MODE_INDIRECT, 3 },       // 0x6C
    { OPCODE_ADC, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x6D
    { OPCODE_ROR, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x6E
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x6F

    { OPCODE_BVS, ADDRESS_MODE_RELATIVE, 2 },       // 0x70
    { OPCODE_ADC, ADDRESS_MODE_INDIRECT_Y, 2 },     // 0x71
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x72
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x73
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x74
    { OPCODE_ADC, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x75
    { OPCODE_ROR, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x76
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x77
    { OPCODE_SEI, ADDRESS_MODE_IMPLIED, 1 },        // 0x78
    { OPCODE_ADC, ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0x79
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x7A
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x7B
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x7C
    { OPCODE_ADC, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x7D
    { OPCODE_ROR, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x7E
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x7F

    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x80
    { OPCODE_STA, ADDRESS_MODE_INDIRECT_X, 2 },     // 0x81
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x82
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x83
    { OPCODE_STY, ADDRESS_MODE_ZEROPAGE, 2 },       // 0x84
    { OPCODE_STA, ADDRESS_MODE_ZEROPAGE, 2 },       // 0x85
    { OPCODE_STX, ADDRESS_MODE_ZEROPAGE, 2 },       // 0x86
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x87
    { OPCODE_DEY, ADDRESS_MODE_IMPLIED, 1 },        // 0x88
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x89
    { OPCODE_TXA, ADDRESS_MODE_IMPLIED, 1 },        // 0x8A
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x8B
    { OPCODE_STY, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x8C
    { OPCODE_STA, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x8D
    { OPCODE_STX, ADDRESS_MODE_ABSOLUTE, 3 },       // 0x8E
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x8F

    { OPCODE_BCC, ADDRESS_MODE_RELATIVE, 2 },       // 0x90
    { OPCODE_STA, ADDRESS_MODE_INDIRECT_Y, 2 },     // 0x91
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x92
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x93
    { OPCODE_STY, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x94
    { OPCODE_STA, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x95
    { OPCODE_STX, ADDRESS_MODE_ZEROPAGE_Y, 2 },     // 0x96
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x97
    { OPCODE_TYA, ADDRESS_MODE_IMPLIED, 1 },        // 0x98
    { OPCODE_STA, ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0x99
    { OPCODE_TXS, ADDRESS_MODE_IMPLIED, 1 },        // 0x9A
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x9B
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x9C
    { OPCODE_STA, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x9D
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x9E
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0x9F

    { OPCODE_LDY, ADDRESS_MODE_IMMEDIATE, 2 },      // 0xA0
    { OPCODE_LDA, ADDRESS_MODE_INDIRECT_X, 2 },     // 0xA1
    { OPCODE_LDX, ADDRESS_MODE_IMMEDIATE, 2 },      // 0xA2
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xA3
    { OPCODE_LDY, ADDRESS_MODE_ZEROPAGE, 2 },       // 0xA4
    { OPCODE_LDA, ADDRESS_MODE_ZEROPAGE, 2 },       // 0xA5
    { OPCODE_LDX, ADDRESS_MODE_ZEROPAGE, 2 },       // 0xA6
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xA7
    { OPCODE_TAY, ADDRESS_MODE_IMPLIED, 1 },        // 0xA8
    { OPCODE_LDA, ADDRESS_MODE_IMMEDIATE, 2 },      // 0xA9
    { OPCODE_TAX, ADDRESS_MODE_IMPLIED, 1 },        // 0xAA
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xAB
    { OPCODE_LDY, ADDRESS_MODE_ABSOLUTE, 3 },       // 0xAC
    { OPCODE_LDA, ADDRESS_MODE_ABSOLUTE, 3 },       // 0xAD
    { OPCODE_LDX, ADDRESS_MODE_ABSOLUTE, 3 },       // 0xAE
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xAF

    { OPCODE_BCS, ADDRESS_MODE_RELATIVE, 2 },       // 0xB0
    { OPCODE_LDA, ADDRESS_MODE_INDIRECT_Y, 2 },     // 0xB1
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xB2
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xB3
    { OPCODE_LDY, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0xB4
    { OPCODE_LDA, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0xB5
    { OPCODE_LDX, ADDRESS_MODE_ZEROPAGE_Y, 2 },     // 0xB6
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xB7
    { OPCODE_CLV, ADDRESS_MODE_IMPLIED, 1 },        // 0xB8
    { OPCODE_LDA, ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0xB9
    { OPCODE_TSX, ADDRESS_MODE_IMPLIED, 1 },        // 0xBA
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xBB
    { OPCODE_LDY, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0xBC
    { OPCODE_LDA, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0xBD
    { OPCODE_LDX, ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0xBE
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xBF

    { OPCODE_CPY, ADDRESS_MODE_IMMEDIATE, 2 },      // 0xC0
    { OPCODE_CMP, ADDRESS_MODE_INDIRECT_X, 2 },     // 0xC1
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xC2
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xC3
    { OPCODE_CPY, ADDRESS_MODE_ZEROPAGE, 2 },       // 0xC4
    { OPCODE_CMP, ADDRESS_MODE_ZEROPAGE, 2 },       // 0xC5
    { OPCODE_DEC, ADDRESS_MODE_ZEROPAGE, 2 },       // 0xC6
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xC7
    { OPCODE_INY, ADDRESS_MODE_IMPLIED, 1 },        // 0xC8
    { OPCODE_CMP, ADDRESS_MODE_IMMEDIATE, 2 },      // 0xC9
    { OPCODE_DEX, ADDRESS_MODE_IMPLIED, 1 },        // 0xCA
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xCB
    { OPCODE_CPY, ADDRESS_MODE_ABSOLUTE, 3 },       // 0xCC
    { OPCODE_CMP, ADDRESS_MODE_ABSOLUTE, 3 },       // 0xCD
    { OPCODE_DEC, ADDRESS_MODE_ABSOLUTE, 3 },       // 0xCE
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xCF

    { OPCODE_BNE, ADDRESS_MODE_RELATIVE, 2 },       // 0xD0
    { OPCODE_CMP, ADDRESS_MODE_INDIRECT_Y, 2 },     // 0xD1
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xD2
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xD3
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xD4
    { OPCODE_CMP, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0xD5
    { OPCODE_DEC, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0xD6
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xD7
    { OPCODE_CLD, ADDRESS_MODE_IMPLIED, 1 },        // 0xD8
    { OPCODE_CMP, ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0xD9
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xDA
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xDB
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xDC
    { OPCODE_CMP, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0xDD
    { OPCODE_DEC, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0xDE
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xDF

    { OPCODE_CPX, ADDRESS_MODE_IMMEDIATE, 2 },      // 0xE0
    { OPCODE_SBC, ADDRESS_MODE_INDIRECT_X, 2 },     // 0xE1
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xE2
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xE3
    { OPCODE_CPX, ADDRESS_MODE_ZEROPAGE, 2 },       // 0xE4
    { OPCODE_SBC, ADDRESS_MODE_ZEROPAGE, 2 },       // 0xE5
    { OPCODE_INC, ADDRESS_MODE_ZEROPAGE, 2 },       // 0xE6
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xE7
    { OPCODE_INX, ADDRESS_MODE_IMPLIED, 1 },        // 0xE8
    { OPCODE_SBC, ADDRESS_MODE_IMMEDIATE, 2 },      // 0xE9
    { OPCODE_NOP, ADDRESS_MODE_IMPLIED, 1 },        // 0xEA
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xEB
    { OPCODE_CPX, ADDRESS_MODE_ABSOLUTE, 3 },       // 0xEC
    { OPCODE_SBC, ADDRESS_MODE_ABSOLUTE, 3 },       // 0xED
    { OPCODE_INC, ADDRESS_MODE_ABSOLUTE, 3 },       // 0xEE
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xEF

    { OPCODE_BEQ, ADDRESS_MODE_RELATIVE, 2 },       // 0xF0
    { OPCODE_SBC, ADDRESS_MODE_INDIRECT_Y, 2 },     // 0xF1
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xF2
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xF3
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xF4
    { OPCODE_SBC, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0xF5
    { OPCODE_INC, ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0xF6
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xF7
    { OPCODE_SED, ADDRESS_MODE_IMPLIED, 1 },        // 0xF8
    { OPCODE_SBC, ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0xF9
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xFA
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xFB
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xFC
    { OPCODE_SBC, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0xFD
    { OPCODE_INC, ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0xFE
    { OPCODE_UNKNOWN, ADDRESS_MODE_UNKNOWN, 0 },    // 0xFF
};




// Decode 4 byte chunk into 6502 CPU instruction
// @param[in] word Chunk to decode
// @returns Decoded instruction
inline instruction_t instruction_decode(u32 word) {
    // The chunk should look like this:
    // 0. -- 24b 1. ----------- 16b   2. ------------- 8b 3. --- 0b
    // [opcode ] [operand (8-bit) ]<=>[operand (16-bit) ] [garbage]

    instruction_t inst;

    // Get the info from the global table
    inst.info = g_instruction_info_table[(word >> 24)];
    
    // Extract the operand maintaining endian-ness based on instruction size
    switch (inst.info.size) {
    case 2: // 8-bit operand
        inst.operand = (u16)((word & 0x00ff0000) >> 16);
        break;
    case 3: // 16-bit operand
        inst.operand = bswap16((u16)(word >> 8));
        break;
    default:
        inst.operand = 0;
    }

    return inst;
}

