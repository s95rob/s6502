#include "cpu.h"

struct cpu_s {
    u8 a, x, y, sp, status;
    u16 pc;
    u64 cycles;
    bus_t* bus;
};


static inline cpu_eval_zero_flag(cpu_t* cpu, u8 result) {
    if (result == 0)
        cpu->status |= CPU_STATUS_FLAG_ZERO_BIT;
}

static inline cpu_eval_negative_flag(cpu_t* cpu, u8 result) {
    if ((i8)result < 0)
        cpu->status |= CPU_STATUS_FLAG_NEGATIVE_BIT;
}


cpu_t* cpu_create(bus_t* bus) {
    assert(bus != NULL);
    
    cpu_t* cpu = (cpu_t*)calloc(1, sizeof(cpu_t));
    cpu->bus = bus;

    return cpu;
}

void cpu_free(cpu_t* cpu) {
    free(cpu);
}


void cpu_exec(cpu_t* cpu, const instruction_t* inst) {
    // TODO: finish opcode implementations
    // TODO: implement cycle count

    switch (inst->info.opcode) {
    case OPCODE_LDA:
        if (inst->info.address_mode == ADDRESS_MODE_IMMEDIATE)
            cpu->a = (u8)inst->operand;
        else
            bus_load(cpu->bus, inst->operand, &cpu->a);

        cpu_eval_zero_flag(cpu, cpu->a);
        cpu_eval_negative_flag(cpu, cpu->a);
        break;
        
    case OPCODE_LDX:
        if (inst->info.address_mode == ADDRESS_MODE_IMMEDIATE)
            cpu->x = (u8)inst->operand;
        else
            bus_load(cpu->bus, inst->operand, &cpu->x);

        cpu_eval_zero_flag(cpu, cpu->x);
        cpu_eval_negative_flag(cpu, cpu->x);
        break;

    case OPCODE_LDY:
        if (inst->info.address_mode == ADDRESS_MODE_IMMEDIATE)
            cpu->y = (u8)inst->operand;
        else
            bus_load(cpu->bus, inst->operand, &cpu->y);

        cpu_eval_zero_flag(cpu, cpu->y);
        cpu_eval_negative_flag(cpu, cpu->y);
        break;
    }

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