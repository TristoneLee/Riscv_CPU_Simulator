#ifndef RISCVCPUSIMULATOR_REGISTER_H
#define RISCVCPUSIMULATOR_REGISTER_H

enum RegStatus {
    available, invalid, valid
};

const int PHYSICAL_REGISTER_MAX = 100;

class Registers {
private:
    int regs[PHYSICAL_REGISTER_MAX];
    RegStatus status[PHYSICAL_REGISTER_MAX];
    int renameMapTable[32];//ISA寄存器目前时间对应的物理寄存器编号
    int occupied;//被哪条指令占用

public:
    int &operator[](int x) {
        return regs[x];
    }

    int reserve_S(int iReg) {
        for (int i = 0; i < PHYSICAL_REGISTER_MAX; ++i)
            if (status[i] == available) {
                renameMapTable[iReg] = i;
                return i;
            }
        return PHYSICAL_REGISTER_MAX;
    }

    int reserve_L(int iReg){
        if(renameMapTable[iReg]!=0) return renameMapTable[iReg];
    }
};


#endif //RISCVCPUSIMULATOR_REGISTER_H
