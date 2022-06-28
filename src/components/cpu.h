#ifndef RISCV_CPU_SIMULATOR_CPU_H
#define RISCV_CPU_SIMULATOR_CPU_H

#include "Memory.h"
#include "Clock.h"
#include "Register.h"
#include "tools.h"
#include "FetchBuffer.h"
#include "LoadStoreUnit.h"
#include "ReorderBuffer.h"

class cpu {
    Memory Mem;
    Registers Reg;
    uint32_t PC;
    FetchBuffer FTB;
    LoadStoreUnit LSU;
    ReorderBuffer RoB;

    void run();
    void Fetch();
    void DecodeRenameDispatch();
    void Dispatch(UOP uop);
    void Issue();
    void Execute();
    void Commit();
    void Fire();
    void RegisterRead();
public:

};


#endif //RISCV_CPU_SIMULATOR_CPU_H
