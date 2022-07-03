#ifndef RISCVCPUSIMULATOR_REORDERBUFFER_HPP
#define RISCVCPUSIMULATOR_REORDERBUFFER_HPP

#include "tools.hpp"
#include "decoder.hpp"
#include "Register.hpp"
#include "LoadStoreUnit.hpp"
#include "IssueQueue.hpp"

const int ROB_MAX = 100;

enum DesType {
    Reg, Mem, PC
};

class ReorderBuffer {
    friend class cpu;

private:
    struct Reorder {
        bool ready = false;
        UOP uop_;
        int result;
    };

    CircularQueue<Reorder, ROB_MAX> buffer_;
    Registers *registers_;
    LoadStoreUnit *load_store_unit_;
    IssueQueue *issue_queue_;
    int *PC_;

public:
    ReorderBuffer(Registers *registers, LoadStoreUnit *load_store_unit, int *PC, IssueQueue *issue_queue) : registers_(
            registers), load_store_unit_(load_store_unit), buffer_(), PC_(PC), issue_queue_(issue_queue) {}

    int push(UOP uop) {
        if (uop.type == U || uop.name == JAL || uop.name == END || uop.name == RET)
            return buffer_.push({true, uop, uop.imm});
        else return buffer_.push({false, uop, 0});
    }

    Reorder &operator[](int num) { return buffer_[num]; }

    Reorder front() { return buffer_.front(); }

    void Commit() {
        Reorder front_reorder = buffer_.front();
        UOP uop = front_reorder.uop_;
        buffer_.pop();
        if (uop.name == END) {
            cout << (((unsigned int) (*registers_)[registers_->register_snapshots.front().rename_map_table_[10]]) &
                     255u) << endl;
            throw 1;
        } else if (uop.name == RET) {
            *PC_ = (*registers_)[registers_->register_snapshots.front().rename_map_table_[1]];
        } else if (uop.type == R || uop.type == I && uop.name != JALR || uop.name == LUI) {
            registers_->Write(front_reorder.result, uop.rd);
            issue_queue_->UpdateStatus(uop.rd);
        } else if (uop.name == AUIPC) {
            *PC_ += front_reorder.result;
            registers_->Write(front_reorder.result, uop.rd);
            issue_queue_->UpdateStatus(uop.rd);
        } else if (uop.name == JAL) {
            registers_->Write(uop.PC + 4, uop.rd);
            issue_queue_->UpdateStatus(uop.rd);
        } else if (uop.name == JALR) {
            *PC_ = (registers_->Read(uop.rs1) + uop.imm) & -1;
            registers_->Write(uop.PC + 4, uop.rd);
            issue_queue_->UpdateStatus(uop.rd);
            registers_->Pop();
            throw "PC_reset";
        } else if (uop.name == BEQ) {
            if (registers_->Read(uop.rs1) == registers_->Read(uop.rs2)) {
                *PC_ = uop.PC + uop.imm;
                registers_->Pop();
                throw "PC_reset";
            } else registers_->Reflush();
        } else if (uop.name == BNE) {
            if (registers_->Read(uop.rs1) != registers_->Read(uop.rs2)) {
                *PC_ = uop.PC + uop.imm;
                registers_->Pop();
                throw "PC_reset";
            } else registers_->Reflush();
        } else if (uop.name == BLT) {
            if (registers_->Read(uop.rs1) < registers_->Read(uop.rs2)) {
                *PC_ = uop.PC + uop.imm;
                registers_->Pop();
                throw "PC_reset";
            } else registers_->Reflush();
        } else if (uop.name == BGE) {
            if (registers_->Read(uop.rs1) >= registers_->Read(uop.rs2)) {
                *PC_ = uop.PC + uop.imm;
                registers_->Pop();
                throw "PC_reset";
            } else registers_->Reflush();
        } else if (uop.name == BLTU) {
            if ((unsigned) registers_->Read(uop.rs1) <= (unsigned) registers_->Read(uop.rs2)) {
                *PC_ = uop.PC + uop.imm;
                registers_->Pop();
                throw "PC_reset";
            } else registers_->Reflush();
        } else if (uop.name == BGEU) {
            if ((unsigned) registers_->Read(uop.rs1) >= (unsigned) registers_->Read(uop.rs2)) {
                *PC_ = uop.PC + uop.imm;
                registers_->Pop();
                throw "PC_reset";
            } else registers_->Reflush();
        } else if (uop.type == S) {
            load_store_unit_->LSU[uop.LSU_pr].ready_ = true;
        }
        // todo 把S提前
    }

    bool empty() { return buffer_.empty(); }

    bool full() { return buffer_.full(); }

    bool ready() { return buffer_.front().ready; }

    void Clear() {
        buffer_.clear();
    }

};

#endif //RISCVCPUSIMULATOR_REORDERBUFFER_HPP
