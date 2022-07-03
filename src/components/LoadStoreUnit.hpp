#ifndef RISCVCPUSIMULATOR_LOADSTOREUNIT_HPP
#define RISCVCPUSIMULATOR_LOADSTOREUNIT_HPP

#include "tools.hpp"
#include "decoder.hpp"
#include "Register.hpp"

const int LSU_MAX = 10;

class LoadStoreUnit {
    friend class cpu;
    friend class ReorderBuffer;

    struct LoadStoreReservation {
        InsName name_ = NONE;
        bool ready_ = 0;
        int des_ = 0;
        int RoB_pr_ = 0;
        int source_=0;

        LoadStoreReservation(InsName name = NONE, bool ready = 0, int des = 0, int RoB_pr = 0,int source=0) :
                name_(name), ready_(ready), des_(des), RoB_pr_(RoB_pr),source_(source) {};
    };

    CircularQueue<LoadStoreReservation, LSU_MAX> LSU;
    Registers *registers_;
    Memory *memory_;

public:
    LoadStoreUnit(Registers *registers, Memory *memory) : LSU(), registers_(), memory_(memory) {}

    int Push(const UOP &uop) {
        return LSU.push({uop.name, false, 0, uop.RoB_pr});
    }

    void Pop(){
        LSU.pop();
    }

    bool Full() { return LSU.full(); }

    bool Empty() {return LSU.empty();}

    int LoadExecution() {
        switch (LSU.front().name_) {
            case LB:
                return memory_->read(LSU.front().des_, 1);
            case LH:
                return memory_->read(LSU.front().des_, 2);
            case LW:
                return memory_->read(LSU.front().des_, 4);
            case LHU:
                return (memory_->read(LSU.front().des_, 2)) & 255u;
            default:
                return (memory_->read(LSU.front().des_, 4)) & 255;
        }
    }

    void StoreExecution() {
        switch (LSU.front().name_) {
            case SB:
                memory_->write(LSU.front().des_, 1, LSU.front().source_);
                break;
            case SH:
                memory_->write(LSU.front().des_, 2, LSU.front().source_);
                break;
            case SW:
                memory_->write(LSU.front().des_, 4, LSU.front().source_);
        }
    }

    void Clear(){
        LSU.clear();
    }
};

#endif //RISCVCPUSIMULATOR_LOADSTOREUNIT_HPP
