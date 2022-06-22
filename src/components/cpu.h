#ifndef RISCV_CPU_SIMULATOR_CPU_H
#define RISCV_CPU_SIMULATOR_CPU_H

#include "Memory.h"
#include "Clock.h"
#include "InstructionQueue.h"

class cpu {
    Memory Mem;
    uint32_t Register[32];
    uint32_t PC;
    InsQueue insQueue;

public:
    void run();

    void execute(Instruction curIns);
};


#endif //RISCV_CPU_SIMULATOR_CPU_H
