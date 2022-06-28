#ifndef RISCVCPUSIMULATOR_FETCHBUFFER_H
#define RISCVCPUSIMULATOR_FETCHBUFFER_H

#include <decoder.h>
#include <queue>

class FetchBuffer {
    std::queue<baseIns> Queue;
    Memory *Mem;
    Registers *Reg;
    uint32_t *PC;


public:
    FetchBuffer(Memory *_Mem, Registers *_Regs, int *_PC) : Mem(_Mem), Reg(_Regs), Queue(), PC(_PC) {};

    void push() {
        baseIns base(Mem->readIns(*PC));
        Queue.push(base);
    };

    baseIns front() { return Queue.front(); }

    void pop() { Queue.pop(); };

}


#endif //RISCVCPUSIMULATOR_FETCHBUFFER_H
