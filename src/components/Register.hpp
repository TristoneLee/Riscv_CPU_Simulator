#ifndef RISCVCPUSIMULATOR_REGISTER_HPP
#define RISCVCPUSIMULATOR_REGISTER_HPP

#include <list>

using std::list;

enum RegStatus {
    available, ready, not_ready
};

extern const int PHYSICAL_REGISTER_MAX = 50;

class Registers {
    friend class cpu;

    friend class IssueQueue;

    friend class ReorderBuffer;

private:
    int regs_[PHYSICAL_REGISTER_MAX] = {0};

    struct RegisterMapSnapshot {
        RegStatus reg_status_[PHYSICAL_REGISTER_MAX];
        int rename_map_table_[32];//ISA寄存器目前时间对应的物理寄存器编号
        int occupied_by_[PHYSICAL_REGISTER_MAX];//被哪条指令占用
        bool if_is_map[PHYSICAL_REGISTER_MAX] = {0};

        RegisterMapSnapshot() = default;
    };

    list<RegisterMapSnapshot> register_snapshots;


public:
    Registers() {
        register_snapshots.push_back(RegisterMapSnapshot());
    }

    int &operator[](int x) {
        return regs_[x];
    }

    int ReserveS(int iReg) {
        for (int i = 1; i < PHYSICAL_REGISTER_MAX; ++i)
            if (register_snapshots.front().reg_status_[i] == available) {
                register_snapshots.front().if_is_map[register_snapshots.front().rename_map_table_[iReg]] = false;
                if (register_snapshots.front().occupied_by_[register_snapshots.front().rename_map_table_[iReg]] == 0)
                    register_snapshots.front().reg_status_[register_snapshots.front().rename_map_table_[iReg]] = available;
                register_snapshots.front().rename_map_table_[iReg] = i;
                register_snapshots.front().if_is_map[i] = true;
                register_snapshots.front().reg_status_[i] = not_ready;
                return i;
            }
        return PHYSICAL_REGISTER_MAX;
    }

    int ReserveL(int iReg) {
        if (register_snapshots.front().rename_map_table_[iReg] == 0) {
            for (int i = 1; i < PHYSICAL_REGISTER_MAX; ++i)
                if (register_snapshots.front().reg_status_[i] == available) {
                    register_snapshots.front().rename_map_table_[iReg] = i;
                    register_snapshots.front().if_is_map[i] = true;
                    register_snapshots.front().reg_status_[i] = ready;
                    regs_[i] = 0;
                    ++register_snapshots.front().occupied_by_[i];
                    return i;
                }
        }
        ++register_snapshots.front().occupied_by_[register_snapshots.front().rename_map_table_[iReg]];
        return register_snapshots.front().rename_map_table_[iReg];
    }

    int Read(int pos) {
        --register_snapshots.front().occupied_by_[pos];
        if (!register_snapshots.front().if_is_map[pos] &&
            register_snapshots.front().occupied_by_[pos] == 0)
            register_snapshots.front().reg_status_[pos] = available;
        return regs_[pos];
    }

    void Write(int value, int pos) {
        register_snapshots.front().reg_status_[pos] = ready;
        regs_[pos] = value;
    }

    void Print() {
        for (int i = 0; i < 32; ++i) cerr << (unsigned) regs_[register_snapshots.front().rename_map_table_[i]] << " ";
        cerr << endl;
    }

    void Take_Shot() {
        register_snapshots.push_back(register_snapshots.front());
    }

    void Reflush() {
        auto iter = register_snapshots.begin();
        iter++;
        register_snapshots.erase(iter);
    }

    void Pop() {
        register_snapshots.pop_front();
        while (register_snapshots.size() > 1) register_snapshots.pop_back();
    }
};


#endif //RISCVCPUSIMULATOR_REGISTER_HPP
