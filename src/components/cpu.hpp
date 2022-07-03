#ifndef RISCVCPUSIMULATOR_CPU_HPP
#define RISCVCPUSIMULATOR_CPU_HPP

#include "Memory.hpp"
#include "Register.hpp"
#include "tools.hpp"
#include "FetchBuffer.hpp"
#include "ReorderBuffer.hpp"
#include "IssueQueue.hpp"
#include "FunctionalUnit.hpp"
#include "LoadStoreUnit.hpp"


class cpu {
private:
    Memory Mem_;
    Registers Reg_;
    int PC_ = 0;
    FetchBuffer fetch_buffer_;
    LoadStoreUnit load_store_unit_;
    ReorderBuffer reorder_buffer_;
    IssueQueue issue_queue_;
    FunctionalUnit functional_unit_;

    void Fetch();

    void DecodeRenameDispatch();

//    void Dispatch(UOP uop);
    void Issue();

    void Execute();

    void Commit();

    void MemoryExecution();

    void Clear();

public:
    cpu() : Mem_(), Reg_(), fetch_buffer_(&Mem_, &PC_), load_store_unit_(&Reg_, &Mem_),
            issue_queue_(&Reg_), reorder_buffer_(&Reg_, &load_store_unit_, &PC_, &issue_queue_),
            functional_unit_() {}

    void run();

};

void cpu::run() {
    int cycle = 0;
    while (true) {
        try {
//            cerr << "PC" << hex << PC_ << "  ";
//            Reg_.Print();
            Fetch();
            DecodeRenameDispatch();
            Issue();
            Execute();
            MemoryExecution();
            Commit();
        }
        catch (int) { break; }
        catch (...) { Clear(); }
    }
}

void cpu::Fetch() {
    BaseIns base(Mem_.readIns(PC_));
    UOP uop(base);
    uop.PC=PC_;
    if (uop.name == JAL) PC_ += uop.imm;
    else { PC_ += 4; }
    fetch_buffer_.push(uop);
}

void cpu::DecodeRenameDispatch() {
    UOP uop = fetch_buffer_.front();
    if (uop.name == END) {
        reorder_buffer_.push(uop);
        fetch_buffer_.pop();
        return;
    } else if (uop.name == RET) {
        fetch_buffer_.pop();
        reorder_buffer_.push(uop);
        return;
    }
    //Rename
    if (uop.type == B||uop.name==JALR) Reg_.Take_Shot();
    if (uop.type == R || uop.type == S || uop.type == B) {
        uop.rs1 = Reg_.ReserveL(uop.rs1);
        uop.rs2 = Reg_.ReserveL(uop.rs2);
    } else if (uop.type == I) {
        uop.rs1 = Reg_.ReserveL(uop.rs1);
    }
    if (uop.type != B && uop.type != S) {
        uop.rd = Reg_.ReserveS(uop.rd);
        if (uop.rd == PHYSICAL_REGISTER_MAX) return;
    }
    fetch_buffer_.pop();
    //if stall
    if (reorder_buffer_.full()) return;
    if (uop.type == S && load_store_unit_.Full()) return;
    if ((uop.name == LB || uop.name == LBU || uop.name == LH || uop.name == LHU || uop.name == LW) &&
        load_store_unit_.Full())
        return;
    //reserve in RoB
    uop.RoB_pr = reorder_buffer_.push(uop);
    //reserve in issueQueue/LSU
    if (uop.type == S || uop.name == LB || uop.name == LBU || uop.name == LH || uop.name == LHU || uop.name == LW)
        uop.LSU_pr = load_store_unit_.Push(uop);
    if (!issue_queue_.Full()) {
        if (uop.type != U && uop.name != JAL)
            issue_queue_.Push(uop);
    }
}

void cpu::Issue() {
    for (int i = 0; i < kIssueQueueMAax; ++i) {
        if (issue_queue_.Empty()) return;
        if (functional_unit_.Full()) break;
        if (issue_queue_[i].issue_status_ == issue_ready) {
            IssueQueue::IssueUnit cur_issue = issue_queue_[i];
            switch (cur_issue.name_) {
                case ADD:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), Reg_.Read(cur_issue.Q2_), Add, cur_issue.RoB_pr);
                    break;
                case ADDI:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), cur_issue.A_, Add, cur_issue.RoB_pr);
                    break;
                case SUB:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), Reg_.Read(cur_issue.Q2_), Sub, cur_issue.RoB_pr);
                    break;
                case XOR:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), Reg_.Read(cur_issue.Q2_), Xor, cur_issue.RoB_pr);
                    break;
                case XORI:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), cur_issue.A_, Xor, cur_issue.RoB_pr);
                    break;
                case OR:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), Reg_.Read(cur_issue.Q2_), Or, cur_issue.RoB_pr);
                    break;
                case ORI:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), cur_issue.A_, Or, cur_issue.RoB_pr);
                    break;
                case AND:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), Reg_.Read(cur_issue.Q2_), And, cur_issue.RoB_pr);
                    break;
                case ANDI:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), cur_issue.A_, And, cur_issue.RoB_pr);
                    break;
                case SLL:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), Reg_.Read(cur_issue.Q2_) & (~(-1 << 5)), Shift_Left,
                                          cur_issue.RoB_pr);
                    break;
                case SLLI:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), cur_issue.A_, Shift_Left, cur_issue.RoB_pr);
                    break;
                case SRL:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), Reg_.Read(cur_issue.Q2_) & (~(-1 << 5)),
                                          Shift_Right_Logical,
                                          cur_issue.RoB_pr);
                    break;
                case SRLI:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), cur_issue.A_, Shift_Right_Logical,
                                          cur_issue.RoB_pr);
                    break;
                case SRA:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), Reg_.Read(cur_issue.Q2_) & (~(-1 << 5)),
                                          Shift_Right_Arithmetic, cur_issue.RoB_pr);
                    break;
                case SRAI:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), cur_issue.A_, Shift_Right_Arithmetic,
                                          cur_issue.RoB_pr);
                    break;
                case SLT:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), Reg_.Read(cur_issue.Q2_), Less_Than,
                                          cur_issue.RoB_pr);
                    break;
                case SLTI:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), sext(cur_issue.A_, 12), Less_Than,
                                          cur_issue.RoB_pr);
                    break;
                case SLTU:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), Reg_.Read(cur_issue.Q2_), Less_Than_Unsigned,
                                          cur_issue.RoB_pr);
                    break;
                case SLTIU:
                    functional_unit_.Push(Reg_.Read(cur_issue.Q1_), sext(cur_issue.A_, 12), Less_Than_Unsigned,
                                          cur_issue.RoB_pr);
                    break;
                case LB:
                case LH:
                case LHU:
                case LW:
                case LBU:
                    load_store_unit_.LSU[cur_issue.LSU_pr].ready_ = true;
                    load_store_unit_.LSU[cur_issue.LSU_pr].des_ = Reg_.Read(cur_issue.Q1_) + sext(cur_issue.A_, 12);
                    break;
                case SB:
                case SH:
                case SW:
//                    reorder_buffer_[cur_issue.RoB_pr].ready = true;
                    reorder_buffer_[cur_issue.RoB_pr].ready = true;
                    reorder_buffer_[cur_issue.RoB_pr].uop_.LSU_pr = cur_issue.LSU_pr;
                    load_store_unit_.LSU[cur_issue.LSU_pr].source_ = Reg_.Read(cur_issue.Q2_);
                    load_store_unit_.LSU[cur_issue.LSU_pr].des_ = Reg_.Read(cur_issue.Q1_) + sext(cur_issue.A_, 12);
                    break;
                case BGEU:
                case BEQ:
                case BNE:
                case BLT:
                case BGE:
                case BLTU:
                case JALR:
                    reorder_buffer_[cur_issue.RoB_pr].ready = true;
            }
            issue_queue_.Pop(i);
        }
    }
}

void cpu::Execute() {
    for (int i = 0; i < ALU_Max; ++i) {
        if (functional_unit_.count == 0) return;
        if (!functional_unit_[i].free_) {
            reorder_buffer_[functional_unit_[i].RoB_pr_].result = functional_unit_.Execute(i);
            reorder_buffer_[functional_unit_[i].RoB_pr_].ready = true;
            functional_unit_[i].free_ = true;
        }
    }
}

void cpu::Commit() {
    while (!reorder_buffer_.empty() && reorder_buffer_.front().ready) {
        reorder_buffer_.Commit();
    }
}

void cpu::MemoryExecution() {
    while (true) {
        if (load_store_unit_.Empty()) break;
        auto cur_op = load_store_unit_.LSU.front();
        if (cur_op.ready_ == false) break;
        switch (cur_op.name_) {
            case LB:
            case LH:
            case LW:
            case LBU:
            case LHU:
                reorder_buffer_[cur_op.RoB_pr_].result = load_store_unit_.LoadExecution();
                reorder_buffer_[cur_op.RoB_pr_].ready = true;
                load_store_unit_.Pop();
                break;
            case SB:
            case SH:
            case SW:
                load_store_unit_.StoreExecution();
                load_store_unit_.Pop();
        }
    }
}

void cpu::Clear() {
    reorder_buffer_.Clear();
    functional_unit_.Clear();
    issue_queue_.Clear();
    load_store_unit_.Clear();
    fetch_buffer_.Clear();
}


#endif //RISCVCPUSIMULATORCPU_HPP
