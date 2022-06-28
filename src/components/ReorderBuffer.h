#ifndef RISCVCPUSIMULATOR_REORDERBUFFER_H
#define RISCVCPUSIMULATOR_REORDERBUFFER_H

#include "tools.h"
#include "decoder.h"

const int ROB_MAX=100;

class ReorderBuffer{
    friend class cpu;
private:
    enum DesType{Reg,Mem};
    struct Reorder{
        bool ready=false;
        DesType desType;
        int PC;
        int des;
    };

    CircularQueue<Reorder,ROB_MAX> buffer;

public:

    int push(UOP uop){
        if(uop.type==S)
        return buffer.push({0,Mem,uop.PC,0});
        else return buffer.push({0,Reg,uop.PC,uop.rd});
    }

    Reorder front(){return buffer.front();}

    void pop(){buffer.pop();}

   bool empty(){return buffer.empty();}

   bool full(){return buffer.full();}

   bool ready() {return buffer.front().ready;}

};

#endif //RISCVCPUSIMULATOR_REORDERBUFFER_H
