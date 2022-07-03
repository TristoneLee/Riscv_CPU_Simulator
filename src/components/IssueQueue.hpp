#ifndef RISCVCPUSIMULATOR_ISSUEQUEUE_HPP
#define RISCVCPUSIMULATOR_ISSUEQUEUE_HPP

#include "tools.hpp"
#include "decoder.hpp"
#include "Register.hpp"

const int kIssueQueueMAax = 100;
const int INTERGER_ISSUE_QUEUE_MAX = 100;
const int MEMORY_ISSUE_QUEUE_MAX = 100;

enum IssueStatus {
    issue_available, issue_ready, issue_not_ready
};

class IssueQueue {
    friend class cpu;

private:
    class IssueUnit {
    public:
        InsName name_ = NONE;
        IssueStatus issue_status_ = issue_available;
        int Q1_ = 0, Q2_ = 0;
        bool R1_ = false, R2_ = false;
        int A_ = 0;  //immediate
        int LSU_pr = 0;
        int RoB_pr = 0;

        IssueUnit() = default;

        IssueUnit(UOP uop) : name_(uop.name) {
            if (uop.type == R || uop.type == B) {
                Q1_ = uop.rs1, Q2_ = uop.rs2;
            } else if (uop.type == I || uop.name == JALR) {
                Q1_ = uop.rs1, Q2_ = 0;
                R2_ = true;
                A_ = uop.imm;
                LSU_pr = uop.LSU_pr;
            } else if (uop.type == S) {
                Q1_ = uop.rs1, Q2_ = uop.rs2;
                A_ = uop.imm;
                LSU_pr = uop.LSU_pr;
            }
            issue_status_ = issue_not_ready;
            RoB_pr = uop.RoB_pr;
        }
    };

    IssueUnit queue_[kIssueQueueMAax];
    Registers *registers;
    int count_;

public:
    IssueQueue(Registers *registers1) : registers(registers1), queue_(), count_(0) {};

    void Push(UOP uop) {
        int available_pos = 0;
        for (available_pos; available_pos < kIssueQueueMAax; ++available_pos) {
            if (queue_[available_pos].issue_status_ == issue_available)
                break;
        }
        IssueUnit cur_issue(uop);
        if (uop.type == R || uop.type == S || uop.type == B) {
            cur_issue.R1_ = registers->register_snapshots.front().reg_status_[uop.rs1] == ready;
            cur_issue.R2_ = registers->register_snapshots.front().reg_status_[uop.rs2] == ready;
            if (cur_issue.R1_ && cur_issue.R2_) cur_issue.issue_status_ = issue_ready;
        } else if (uop.type == I || uop.name == JALR) {
            if ((cur_issue.R1_ =
                         registers->register_snapshots.front().reg_status_[uop.rs1] == ready))
                cur_issue.issue_status_ = issue_ready;
        }
        queue_[available_pos] = cur_issue;
        ++count_;
    }

    void Pop(int num) {
        queue_[num].issue_status_ = issue_available;
        --count_;
    }

    void UpdateStatus(int reg_name) {
        for (int i = 0; i < kIssueQueueMAax; ++i) {
            if (queue_[i].issue_status_ != issue_not_ready) continue;
            if (queue_[i].Q1_ == reg_name) queue_[i].R1_ = true;
            if (queue_[i].Q2_ == reg_name) queue_[i].R2_ = true;
            if (queue_[i].R1_ && queue_[i].R2_) queue_[i].issue_status_ = issue_ready;
        }
    }

    bool Empty() { return count_ == 0; }

    bool Full() const { return count_ == kIssueQueueMAax; }

    IssueUnit &operator[](int x) { return queue_[x]; }

    void Clear() {
        for (int i = 0; i < kIssueQueueMAax; ++i) queue_[i].issue_status_ = issue_available;
    }
};

#endif //RISCVCPUSIMULATOR_ISSUEQUEUE_HPP
