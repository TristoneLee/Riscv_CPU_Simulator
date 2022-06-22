#ifndef RISCVCPUSIMULATOR_INSTRUCTIONQUEUE_H
#define RISCVCPUSIMULATOR_INSTRUCTIONQUEUE_H

#include <queue>
#include "decoder.h"
#include "Memory.h"
class InsQueue{
    std::queue<Instruction> Queue;
    Memory *Mem;
     uint32_t *Reg;
    uint32_t *PC;


public:
    InsQueue(Memory *_Mem,uint32_t *_Regs,uint32_t *_PC):Mem(_Mem),Reg(_Regs),Queue(),PC(_PC){};

    void push(){
        baseIns base(Mem->readIns(*PC));
        Instruction newIns(base);
        *PC+=4;
        Queue.push(newIns);
    };

    Instruction pop(){
        Instruction ans=Queue.front();
        Queue.pop();
        return ans;
    };
};


#endif //RISCVCPUSIMULATOR_INSTRUCTIONQUEUE_H
