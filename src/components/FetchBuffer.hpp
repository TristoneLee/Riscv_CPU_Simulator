#ifndef RISCVCPUSIMULATOR_FETCHBUFFER_HPP
#define RISCVCPUSIMULATOR_FETCHBUFFER_HPP

#include <decoder.hpp>
#include <queue>
#include "Memory.hpp"
#include "tools.hpp"

const int FetchBufferMax=20;

class FetchBuffer {
    CircularQueue<UOP,FetchBufferMax> Queue_;
    Memory *Mem_;
    int *PC_;


public:
    FetchBuffer(Memory *Mem,int *PC) : Mem_(Mem),  Queue_(), PC_(PC) {};

    void push(const UOP &uop) {
        Queue_.push(uop);
    };

    UOP front() { return Queue_.front(); }

    void pop() { Queue_.pop(); };

    void Clear(){
        Queue_.clear();
    }

};


#endif //RISCVCPUSIMULATOR_FETCHBUFFER_HPP
