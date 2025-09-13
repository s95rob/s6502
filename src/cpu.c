#define CPU_PRIVATE_IMPLEMENTATION
#include "cpu.h"

struct cpu_s {
    u8 a, x, y, sp, status;
    u16 pc;
    u64 cycles;
    bus_t* bus;
};

static const cpu_instruction_info_t g_cpu_instruction_info_table[256];


// Utilities

static inline void cpu_eval_status(cpu_t* cpu, cpu_status_flags flag, u8 result) {
    if (result)
        cpu->status |= flag;
    else 
        cpu->status &= ~flag;
}

static inline void cpu_eval_zero_flag(cpu_t* cpu, u8 value) {
    if (value == 0)
        cpu->status |= CPU_STATUS_FLAG_ZERO_BIT;
    else
        cpu->status &= ~CPU_STATUS_FLAG_ZERO_BIT;
}

static inline void cpu_eval_negative_flag(cpu_t* cpu, u8 value) {
    if ((i8)value < 0)
        cpu->status |= CPU_STATUS_FLAG_NEGATIVE_BIT;
    else
        cpu->status &= ~CPU_STATUS_FLAG_NEGATIVE_BIT;
    
}

static inline void cpu_eval_carry_flag(cpu_t* cpu, u8 value) {
    u8 result = value >> CPU_STATUS_CARRY_INDEX;
    cpu->status |= (result)
        ? CPU_STATUS_FLAG_CARRY_BIT
        : ~CPU_STATUS_FLAG_CARRY_BIT;
}

// @returns True if the hi-byte of `b` is different than `a`
static inline b8 eval_page_boundary(u16 a, u16 b) {
    return ((a & 0xff00) != (b & 0xff00));
}

// Apply addressing mode to a given address
// @param[in] addr_mode
// @param[out] address
// @returns Additional cycle cost from address mode resolution
static inline u32 cpu_resolve_address(cpu_t* cpu, cpu_address_mode addr_mode, u16* addr) {
    u16 old_addr = *addr;

    switch (addr_mode) {
    case  CPU_ADDRESS_MODE_ZEROPAGE:
        *addr = old_addr % 0xff;
        break;
    case  CPU_ADDRESS_MODE_ZEROPAGE_X:
        *addr = (old_addr + cpu->x) % 0xff;
        break;
    case  CPU_ADDRESS_MODE_ZEROPAGE_Y:
        *addr = (old_addr + cpu->y) % 0xff;
        break;
    case  CPU_ADDRESS_MODE_ABSOLUTE_X:
        *addr = (old_addr + cpu->x);
        return eval_page_boundary(old_addr, *addr);
    case  CPU_ADDRESS_MODE_ABSOLUTE_Y:
        *addr = (old_addr + cpu->y);
        return eval_page_boundary(old_addr, *addr);
    case  CPU_ADDRESS_MODE_INDIRECT_X: {
        u16 zp_addr = (old_addr + cpu->x) % 0xff;
        u8 ptr_lo = 0, 
           ptr_hi = 0;
        bus_load(cpu->bus, zp_addr, &ptr_lo);
        bus_load(cpu->bus, zp_addr + 1, &ptr_hi);
        *addr = (ptr_hi << 8) + ptr_lo;
        break;
    }
    case  CPU_ADDRESS_MODE_INDIRECT_Y: {
        u8 ptr_lo = 0,
           ptr_hi = 0;
        bus_load(cpu->bus, old_addr, &ptr_lo);
        bus_load(cpu->bus, old_addr + 1, &ptr_hi);
        *addr = ((ptr_hi << 8) + ptr_lo) + cpu->y;
        break;
    }
    default:
        break;
    }

    return 0;
}

// Add CPU cycles based on address mode cycle cost
#define CPU_ADD_CYCLES(imm, zp, zp_x, zp_y, abs, abs_x, abs_y, ind_x, ind_y) \
    switch (inst.info.address_mode) { \
    case CPU_ADDRESS_MODE_ZEROPAGE: cycles += zp; break; \
    case CPU_ADDRESS_MODE_ZEROPAGE_X: cycles += zp_x; break; \
    case CPU_ADDRESS_MODE_ZEROPAGE_Y: cycles += zp_y; break; \
    case CPU_ADDRESS_MODE_ABSOLUTE: cycles += abs; break; \
    case CPU_ADDRESS_MODE_ABSOLUTE_X: cycles += abs_x; break; \
    case CPU_ADDRESS_MODE_ABSOLUTE_Y: cycles += abs_y; break; \
    case CPU_ADDRESS_MODE_INDIRECT_X: cycles += ind_x; break; \
    case CPU_ADDRESS_MODE_INDIRECT_Y: cycles += ind_y; break; \
    default: cycles = imm; }


cpu_t* cpu_create(bus_t* bus) {
    assert(bus != NULL);
    
    cpu_t* cpu = (cpu_t*)calloc(1, sizeof(cpu_t));
    cpu->bus = bus;

    return cpu;
}

void cpu_free(cpu_t* cpu) {
    free(cpu);
}

cpu_instruction_t cpu_decode(cpu_t* cpu, u32 word) {
    // The chunk should look like this:
    // 0. -- 24b 1. ----------- 16b   2. ------------- 8b 3. --- 0b
    // [opcode ] [operand (8-bit) ]<=>[operand (16-bit) ] [garbage]

    cpu_instruction_t inst;

    // Get the info from the global table. The opcode is the info's index
    inst.info = g_cpu_instruction_info_table[(word >> 24)];
    
    // Extract the operand maintaining endian-ness, based on instruction size
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

void cpu_exec(cpu_t* cpu, cpu_instruction_t inst) {
    u32 cycles = cpu_resolve_address(cpu, inst.info.address_mode, &inst.operand);

    switch (inst.info.opcode) {
    case CPU_OPCODE_ADC:
        // TODO
        break;
    case CPU_OPCODE_AND:
        CPU_ADD_CYCLES(2, 3, 4, 0, 4, 4, 4, 6, 5);
        if (inst.info.address_mode != CPU_ADDRESS_MODE_IMMEDIATE)
            bus_load(cpu->bus, inst.operand, &inst.operand);

        cpu->a = cpu->a & inst.operand;

        cpu_eval_zero_flag(cpu, cpu->a);
        cpu_eval_negative_flag(cpu, cpu->a);
        break;
    case CPU_OPCODE_ASL:
        CPU_ADD_CYCLES(2, 5, 6, 0, 6, 7, 0, 0, 0);
        if (inst.info.address_mode == CPU_ADDRESS_MODE_ACCUMULATOR) {
            cpu_eval_carry_flag(cpu, cpu->a);
            cpu->a << 1;
        }
        else {
            u8 val = 0;
            bus_load(cpu->bus, inst.operand, &val);
            cpu_eval_carry_flag(cpu, val);
            val << 1;
            bus_store(cpu->bus, inst.operand, val);
        }

        break;
    case CPU_OPCODE_BCC:
        cycles += 2;
        if (!cpu->status & CPU_STATUS_FLAG_CARRY_BIT) {
            cpu->pc += (i16)inst.operand;
            cycles++;
        }

        break;
    case CPU_OPCODE_BCS:
        cycles += 2;
        if (cpu->status & CPU_STATUS_FLAG_CARRY_BIT) {
            cpu->pc += (i16)inst.operand;
            cycles++;
        }

        break;
    case CPU_OPCODE_BEQ:
        cycles += 2;
        if (cpu->status & CPU_STATUS_FLAG_ZERO_BIT) {
            cpu->pc += (i16)inst.operand;
            cycles++;
        }
        
        break;
    case CPU_OPCODE_BIT:
        CPU_ADD_CYCLES(0, 2, 0, 0, 4, 0, 0, 0, 0);
        u8 val;
        bus_load(cpu, inst.operand, &val);
        cpu->status |= val & BIT(6);
        cpu->status |= val & BIT(7);

        cpu_eval_zero_flag(cpu, cpu->a & val);
        break;
    case CPU_OPCODE_BMI:
        cycles += 2;
        if (cpu->status & CPU_STATUS_FLAG_NEGATIVE_BIT) {
            cpu->pc += (i16)inst.operand;
            cycles++;
        }

        break;
    case CPU_OPCODE_BNE:
        cycles += 2;
        if (!(cpu->status & CPU_STATUS_FLAG_ZERO_BIT)) {
            cpu->pc += (i16)inst.operand;
            cycles++;
        }

        break;
    case CPU_OPCODE_BPL:
        cycles += 2;
        if (!(cpu->status & CPU_STATUS_FLAG_NEGATIVE_BIT)) {
            cpu->pc += (i16)inst.operand;
            cycles++;
        }

        break;
    case CPU_OPCODE_BRK:
        cycles = 7;
        cpu_push(cpu, cpu->pc);
        cpu_push(cpu, cpu->status);

        u8 ptr_lo = 0,
           ptr_hi = 0;
        bus_load(cpu, 0xfffe, &ptr_lo);
        bus_load(cpu, 0xffff, &ptr_hi);

        cpu->pc = (ptr_hi << 8) + ptr_lo;

        break;
    case CPU_OPCODE_BVC:
        cycles += 2;
        if (!cpu->status & CPU_STATUS_FLAG_OVERFLOW_BIT) {
            cpu->pc += (i16)inst.operand;
            cycles++;
        }

        break;
    case CPU_OPCODE_BVS:
        cycles += 2;
        if (cpu->status & CPU_STATUS_FLAG_OVERFLOW_BIT) {
            cpu->pc += (i16)inst.operand;
            cycles++;
        }
        
        break;
    case CPU_OPCODE_CLC:
        cycles += 2;
        cpu->status &= ~CPU_STATUS_FLAG_CARRY_BIT;

        break;
    case CPU_OPCODE_CLD:
        cycles += 2;
        cpu->status &= ~CPU_STATUS_FLAG_DECIMAL_BIT;

        break;
    case CPU_OPCODE_CLI:
        cycles += 2;
        cpu->status &= ~CPU_STATUS_FLAG_INTERRUPT_DISABLED_BIT;

        break;
    case CPU_OPCODE_CLV:
        cycles += 2;
        cpu->status &= ~CPU_STATUS_FLAG_OVERFLOW_BIT;

        break;
    case CPU_OPCODE_CMP:
        CPU_ADD_CYCLES(2, 3, 4, 0, 4, 4, 4, 6, 5);

        u8 m = inst.operand;
        if (inst.info.address_mode != CPU_ADDRESS_MODE_IMMEDIATE)
            bus_load(cpu->bus, inst.operand, &m);

        cpu_eval_status(cpu, CPU_STATUS_FLAG_CARRY_BIT, cpu->a >= m);
        cpu_eval_status(cpu, CPU_STATUS_FLAG_ZERO_BIT, cpu->a == m);
        cpu_eval_status(cpu, CPU_STATUS_FLAG_NEGATIVE_BIT, (i8)(cpu->a - m) < 0);
        break;
    case CPU_OPCODE_CPX:
        CPU_ADD_CYCLES(2, 3, 0, 0, 4, 0, 0, 0, 0);

        u8 m = inst.operand;
        if (inst.info.address_mode != CPU_ADDRESS_MODE_IMMEDIATE)
            bus_load(cpu->bus, inst.operand, &m);

        cpu_eval_status(cpu, CPU_STATUS_FLAG_CARRY_BIT, cpu->x >= m);
        cpu_eval_status(cpu, CPU_STATUS_FLAG_ZERO_BIT, cpu->x == m);
        cpu_eval_status(cpu, CPU_STATUS_FLAG_NEGATIVE_BIT, (i8)(cpu->x - m) < 0);
        break;
    case CPU_OPCODE_CPY:
        CPU_ADD_CYCLES(2, 3, 0, 0, 4, 0, 0, 0, 0);

        u8 m = inst.operand;
        if (inst.info.address_mode != CPU_ADDRESS_MODE_IMMEDIATE)
            bus_load(cpu->bus, inst.operand, &m);

        cpu_eval_status(cpu, CPU_STATUS_FLAG_CARRY_BIT, cpu->y >= m);
        cpu_eval_status(cpu, CPU_STATUS_FLAG_ZERO_BIT, cpu->y == m);
        cpu_eval_status(cpu, CPU_STATUS_FLAG_NEGATIVE_BIT, (i8)(cpu->y - m) < 0);
        break;
    case CPU_OPCODE_DEC:
        CPU_ADD_CYCLES(0, 5, 6, 0, 6, 7, 0, 0, 0);

        u8 m = 0;
        bus_load(cpu, inst.operand, &m);
        m -= 1;
        bus_store(cpu, inst.operand, m);

        cpu_eval_status(cpu, CPU_STATUS_FLAG_ZERO_BIT, m == 0);
        cpu_eval_status(cpu, CPU_STATUS_FLAG_NEGATIVE_BIT, (i8)m < 0);
        break;
    case CPU_OPCODE_LDA:
        CPU_ADD_CYCLES(2, 3, 4, 0, 4, 3, 3, 2, 2);

        if (inst.info.address_mode == CPU_ADDRESS_MODE_IMMEDIATE)
            cpu->a = (u8)inst.operand;
        else
            bus_load(cpu->bus, inst.operand, &cpu->a);

        cpu_eval_zero_flag(cpu, cpu->a);
        cpu_eval_negative_flag(cpu, cpu->a);
        break;
    case CPU_OPCODE_LDX:
        CPU_ADD_CYCLES(2, 3, 0, 4, 4, 0, 4, 0, 0);
        
        if (inst.info.address_mode == CPU_ADDRESS_MODE_IMMEDIATE)
            cpu->x = (u8)inst.operand;
        else
            bus_load(cpu->bus, inst.operand, &cpu->a);

        cpu_eval_zero_flag(cpu, cpu->x);
        cpu_eval_negative_flag(cpu, cpu->x);
        break;
    case CPU_OPCODE_LDY:
        CPU_ADD_CYCLES(2, 3, 4, 0, 4, 4, 0, 0, 0);
        
        if (inst.info.address_mode == CPU_ADDRESS_MODE_IMMEDIATE)
            cpu->y = (u8)inst.operand;
        else
            bus_load(cpu->bus, inst.operand, &cpu->a);

        cpu_eval_zero_flag(cpu, cpu->y);
        cpu_eval_negative_flag(cpu, cpu->y);
        break;
    case CPU_OPCODE_STA:
        CPU_ADD_CYCLES(0, 3, 4, 0, 4, 5, 5, 6, 6);
        
        bus_store(cpu->bus, inst.operand, cpu->a);

        break;
    case CPU_OPCODE_STX:
        CPU_ADD_CYCLES(0, 3, 0, 4, 4, 0, 0, 0, 0);
        
        bus_store(cpu->bus, inst.operand, cpu->x);

        break;
    case CPU_OPCODE_STY:
        CPU_ADD_CYCLES(0, 3, 4, 0, 4, 0, 0, 0, 0);
        
        bus_store(cpu->bus, inst.operand, cpu->y);

        break;
    case CPU_OPCODE_TAX:
        cycles = 2;

        cpu->x = cpu->a;

        cpu_eval_zero_flag(cpu, cpu->x);
        cpu_eval_negative_flag(cpu, cpu->x);
        break;
    case CPU_OPCODE_TAY:
        cycles = 2;

        cpu->y = cpu->a;

        cpu_eval_zero_flag(cpu, cpu->y);
        cpu_eval_negative_flag(cpu, cpu->y);
        break;
    case CPU_OPCODE_TSX:
        cycles = 2;

        cpu->x = cpu->sp;

        cpu_eval_zero_flag(cpu, cpu->x);
        cpu_eval_negative_flag(cpu, cpu->x);
        break;
    case CPU_OPCODE_TXA:
        cycles = 2;

        cpu->a = cpu->x;

        cpu_eval_zero_flag(cpu, cpu->a);
        cpu_eval_negative_flag(cpu, cpu->a);
        break;
    case CPU_OPCODE_TXS:
        cycles = 2;

        cpu->sp = cpu->x;

        break;
    case CPU_OPCODE_TYA:
        cycles = 2;

        cpu->a = cpu->y;

        cpu_eval_zero_flag(cpu, cpu->a);
        cpu_eval_negative_flag(cpu, cpu->a);
        break;
    }

    cpu->cycles += cycles;
}

void cpu_push(cpu_t* cpu, u8 value) {
    bus_store(cpu->bus, cpu->sp--, value);
}

u8 cpu_pop(cpu_t* cpu) {
    u8 value = 0;
    bus_load(cpu->bus, cpu->sp++, &value);
    return value;
}


void cpu_get_state(cpu_t* cpu, u8* a, u8* x, u8* y, u8* sp, u8* status, u16* pc, u64* cycles) {
    if (a != NULL)
        *a = cpu->a;
    if (x != NULL)
        *x = cpu->x;
    if (y != NULL)
        *y = cpu->y;
    if (sp != NULL)
        *sp = cpu->sp;
    if (status != NULL)
        *status = cpu->status;
    if (pc != NULL)
        *pc = cpu->pc;
    if (cycles != NULL)
        *cycles = cpu->cycles;
}



static const cpu_instruction_info_t g_cpu_instruction_info_table[256] = {
    { CPU_OPCODE_BRK, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x00
    { CPU_OPCODE_ORA, CPU_ADDRESS_MODE_INDIRECT_X, 2 },     // 0x01
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x02
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x03
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x04
    { CPU_OPCODE_ORA, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0x05
    { CPU_OPCODE_ASL, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0x06
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x07
    { CPU_OPCODE_PHP, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x08
    { CPU_OPCODE_ORA, CPU_ADDRESS_MODE_IMMEDIATE, 2 },      // 0x09
    { CPU_OPCODE_ASL, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x0A
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x0B
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x0C
    { CPU_OPCODE_ORA, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x0D
    { CPU_OPCODE_ASL, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x0E
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x0F

    { CPU_OPCODE_BPL, CPU_ADDRESS_MODE_RELATIVE, 2 },       // 0x10
    { CPU_OPCODE_ORA, CPU_ADDRESS_MODE_INDIRECT_Y, 2 },     // 0x11
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x12
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x13
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x14
    { CPU_OPCODE_ORA, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x15
    { CPU_OPCODE_ASL, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x16
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x17
    { CPU_OPCODE_CLC, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x18
    { CPU_OPCODE_ORA, CPU_ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0x19
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x1A
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x1B
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x1C
    { CPU_OPCODE_ORA, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x1D
    { CPU_OPCODE_ASL, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x1E
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x1F

    { CPU_OPCODE_JSR, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x20
    { CPU_OPCODE_AND, CPU_ADDRESS_MODE_INDIRECT_X, 2 },     // 0x21
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x22
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x23
    { CPU_OPCODE_BIT, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0x24
    { CPU_OPCODE_AND, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0x25
    { CPU_OPCODE_ROL, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0x26
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x27
    { CPU_OPCODE_PLP, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x28
    { CPU_OPCODE_AND, CPU_ADDRESS_MODE_IMMEDIATE, 2 },      // 0x29
    { CPU_OPCODE_ROL, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x2A
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x2B
    { CPU_OPCODE_BIT, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x2C
    { CPU_OPCODE_AND, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x2D
    { CPU_OPCODE_ROL, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x2E
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x2F

    { CPU_OPCODE_BMI, CPU_ADDRESS_MODE_RELATIVE, 2 },       // 0x30
    { CPU_OPCODE_AND, CPU_ADDRESS_MODE_INDIRECT_Y, 2 },     // 0x31
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x32
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x33
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x34
    { CPU_OPCODE_AND, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x35
    { CPU_OPCODE_ROL, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x36
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x37
    { CPU_OPCODE_SEC, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x38
    { CPU_OPCODE_AND, CPU_ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0x39
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x3A
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x3B
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x3C
    { CPU_OPCODE_AND, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x3D
    { CPU_OPCODE_ROL, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x3E
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x3F

    { CPU_OPCODE_RTI, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x40
    { CPU_OPCODE_EOR, CPU_ADDRESS_MODE_INDIRECT_X, 2 },     // 0x41
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x42
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x43
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x44
    { CPU_OPCODE_EOR, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0x45
    { CPU_OPCODE_LSR, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0x46
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x47
    { CPU_OPCODE_PHA, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x48
    { CPU_OPCODE_EOR, CPU_ADDRESS_MODE_IMMEDIATE, 2 },      // 0x49
    { CPU_OPCODE_LSR, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x4A
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x4B
    { CPU_OPCODE_JMP, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x4C
    { CPU_OPCODE_EOR, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x4D
    { CPU_OPCODE_LSR, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x4E
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x4F

    { CPU_OPCODE_BVC, CPU_ADDRESS_MODE_RELATIVE, 2 },       // 0x50
    { CPU_OPCODE_EOR, CPU_ADDRESS_MODE_INDIRECT_Y, 2 },     // 0x51
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x52
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x53
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x54
    { CPU_OPCODE_EOR, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x55
    { CPU_OPCODE_LSR, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x56
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x57
    { CPU_OPCODE_CLI, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x58
    { CPU_OPCODE_EOR, CPU_ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0x59
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x5A
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x5B
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x5C
    { CPU_OPCODE_EOR, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x5D
    { CPU_OPCODE_LSR, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x5E
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x5F

    { CPU_OPCODE_RTS, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x60
    { CPU_OPCODE_ADC, CPU_ADDRESS_MODE_INDIRECT_X, 2 },     // 0x61
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x62
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x63
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x64
    { CPU_OPCODE_ADC, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0x65
    { CPU_OPCODE_ROR, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0x66
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x67
    { CPU_OPCODE_PLA, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x68
    { CPU_OPCODE_ADC, CPU_ADDRESS_MODE_IMMEDIATE, 2 },      // 0x69
    { CPU_OPCODE_ROR, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x6A
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x6B
    { CPU_OPCODE_JMP, CPU_ADDRESS_MODE_INDIRECT, 3 },       // 0x6C
    { CPU_OPCODE_ADC, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x6D
    { CPU_OPCODE_ROR, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x6E
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x6F

    { CPU_OPCODE_BVS, CPU_ADDRESS_MODE_RELATIVE, 2 },       // 0x70
    { CPU_OPCODE_ADC, CPU_ADDRESS_MODE_INDIRECT_Y, 2 },     // 0x71
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x72
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x73
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x74
    { CPU_OPCODE_ADC, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x75
    { CPU_OPCODE_ROR, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x76
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x77
    { CPU_OPCODE_SEI, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x78
    { CPU_OPCODE_ADC, CPU_ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0x79
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x7A
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x7B
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x7C
    { CPU_OPCODE_ADC, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x7D
    { CPU_OPCODE_ROR, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x7E
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x7F

    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x80
    { CPU_OPCODE_STA, CPU_ADDRESS_MODE_INDIRECT_X, 2 },     // 0x81
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x82
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x83
    { CPU_OPCODE_STY, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0x84
    { CPU_OPCODE_STA, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0x85
    { CPU_OPCODE_STX, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0x86
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x87
    { CPU_OPCODE_DEY, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x88
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x89
    { CPU_OPCODE_TXA, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x8A
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x8B
    { CPU_OPCODE_STY, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x8C
    { CPU_OPCODE_STA, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x8D
    { CPU_OPCODE_STX, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0x8E
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x8F

    { CPU_OPCODE_BCC, CPU_ADDRESS_MODE_RELATIVE, 2 },       // 0x90
    { CPU_OPCODE_STA, CPU_ADDRESS_MODE_INDIRECT_Y, 2 },     // 0x91
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x92
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x93
    { CPU_OPCODE_STY, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x94
    { CPU_OPCODE_STA, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0x95
    { CPU_OPCODE_STX, CPU_ADDRESS_MODE_ZEROPAGE_Y, 2 },     // 0x96
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x97
    { CPU_OPCODE_TYA, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x98
    { CPU_OPCODE_STA, CPU_ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0x99
    { CPU_OPCODE_TXS, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0x9A
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x9B
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x9C
    { CPU_OPCODE_STA, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0x9D
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x9E
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0x9F

    { CPU_OPCODE_LDY, CPU_ADDRESS_MODE_IMMEDIATE, 2 },      // 0xA0
    { CPU_OPCODE_LDA, CPU_ADDRESS_MODE_INDIRECT_X, 2 },     // 0xA1
    { CPU_OPCODE_LDX, CPU_ADDRESS_MODE_IMMEDIATE, 2 },      // 0xA2
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xA3
    { CPU_OPCODE_LDY, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0xA4
    { CPU_OPCODE_LDA, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0xA5
    { CPU_OPCODE_LDX, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0xA6
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xA7
    { CPU_OPCODE_TAY, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0xA8
    { CPU_OPCODE_LDA, CPU_ADDRESS_MODE_IMMEDIATE, 2 },      // 0xA9
    { CPU_OPCODE_TAX, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0xAA
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xAB
    { CPU_OPCODE_LDY, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0xAC
    { CPU_OPCODE_LDA, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0xAD
    { CPU_OPCODE_LDX, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0xAE
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xAF

    { CPU_OPCODE_BCS, CPU_ADDRESS_MODE_RELATIVE, 2 },       // 0xB0
    { CPU_OPCODE_LDA, CPU_ADDRESS_MODE_INDIRECT_Y, 2 },     // 0xB1
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xB2
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xB3
    { CPU_OPCODE_LDY, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0xB4
    { CPU_OPCODE_LDA, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0xB5
    { CPU_OPCODE_LDX, CPU_ADDRESS_MODE_ZEROPAGE_Y, 2 },     // 0xB6
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xB7
    { CPU_OPCODE_CLV, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0xB8
    { CPU_OPCODE_LDA, CPU_ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0xB9
    { CPU_OPCODE_TSX, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0xBA
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xBB
    { CPU_OPCODE_LDY, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0xBC
    { CPU_OPCODE_LDA, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0xBD
    { CPU_OPCODE_LDX, CPU_ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0xBE
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xBF

    { CPU_OPCODE_CPY, CPU_ADDRESS_MODE_IMMEDIATE, 2 },      // 0xC0
    { CPU_OPCODE_CMP, CPU_ADDRESS_MODE_INDIRECT_X, 2 },     // 0xC1
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xC2
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xC3
    { CPU_OPCODE_CPY, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0xC4
    { CPU_OPCODE_CMP, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0xC5
    { CPU_OPCODE_DEC, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0xC6
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xC7
    { CPU_OPCODE_INY, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0xC8
    { CPU_OPCODE_CMP, CPU_ADDRESS_MODE_IMMEDIATE, 2 },      // 0xC9
    { CPU_OPCODE_DEX, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0xCA
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xCB
    { CPU_OPCODE_CPY, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0xCC
    { CPU_OPCODE_CMP, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0xCD
    { CPU_OPCODE_DEC, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0xCE
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xCF

    { CPU_OPCODE_BNE, CPU_ADDRESS_MODE_RELATIVE, 2 },       // 0xD0
    { CPU_OPCODE_CMP, CPU_ADDRESS_MODE_INDIRECT_Y, 2 },     // 0xD1
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xD2
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xD3
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xD4
    { CPU_OPCODE_CMP, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0xD5
    { CPU_OPCODE_DEC, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0xD6
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xD7
    { CPU_OPCODE_CLD, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0xD8
    { CPU_OPCODE_CMP, CPU_ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0xD9
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xDA
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xDB
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xDC
    { CPU_OPCODE_CMP, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0xDD
    { CPU_OPCODE_DEC, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0xDE
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xDF

    { CPU_OPCODE_CPX, CPU_ADDRESS_MODE_IMMEDIATE, 2 },      // 0xE0
    { CPU_OPCODE_SBC, CPU_ADDRESS_MODE_INDIRECT_X, 2 },     // 0xE1
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xE2
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xE3
    { CPU_OPCODE_CPX, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0xE4
    { CPU_OPCODE_SBC, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0xE5
    { CPU_OPCODE_INC, CPU_ADDRESS_MODE_ZEROPAGE, 2 },       // 0xE6
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xE7
    { CPU_OPCODE_INX, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0xE8
    { CPU_OPCODE_SBC, CPU_ADDRESS_MODE_IMMEDIATE, 2 },      // 0xE9
    { CPU_OPCODE_NOP, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0xEA
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xEB
    { CPU_OPCODE_CPX, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0xEC
    { CPU_OPCODE_SBC, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0xED
    { CPU_OPCODE_INC, CPU_ADDRESS_MODE_ABSOLUTE, 3 },       // 0xEE
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xEF

    { CPU_OPCODE_BEQ, CPU_ADDRESS_MODE_RELATIVE, 2 },       // 0xF0
    { CPU_OPCODE_SBC, CPU_ADDRESS_MODE_INDIRECT_Y, 2 },     // 0xF1
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xF2
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xF3
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xF4
    { CPU_OPCODE_SBC, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0xF5
    { CPU_OPCODE_INC, CPU_ADDRESS_MODE_ZEROPAGE_X, 2 },     // 0xF6
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xF7
    { CPU_OPCODE_SED, CPU_ADDRESS_MODE_IMPLIED, 1 },        // 0xF8
    { CPU_OPCODE_SBC, CPU_ADDRESS_MODE_ABSOLUTE_Y, 3 },     // 0xF9
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xFA
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xFB
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xFC
    { CPU_OPCODE_SBC, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0xFD
    { CPU_OPCODE_INC, CPU_ADDRESS_MODE_ABSOLUTE_X, 3 },     // 0xFE
    { CPU_OPCODE_UNKNOWN, CPU_ADDRESS_MODE_UNKNOWN, 0 },    // 0xFF
};