#include "cpu.h"

void cpu::run() {
    while (true) {
        try {
            Fetch();
        }
        catch (...) { break; }
    }
}

//void cpu::execute(Instruction curIns) {
//    uint8_t rd = curIns.rd, rs1 = curIns.rs1, rs2 = curIns.rs2;
//    uint32_t imm = curIns.imm;
//    switch (curIns.name) {
//        case LUI: {
//            Reg[rd] = imm << 12;
//        }
//        case AUIPC: {
//            Reg[rd] = PC + (imm << 12);
//        }
//        case JAL: {
//            Reg[rd] = PC + 4;
//            PC += sext(imm, 20);
//        }
//        case JALR: {
//            Reg[rd] = PC + 4;
//            PC = (Reg[rs1] + sext(imm, 12)) & ~1;
//        }
//        case BEQ: {
//            if (Reg[rs1] == Reg[rs2]) PC += sext(imm, 12);
//        }
//        case BNE: {
//            if (Reg[rs1] != Reg[rs2]) PC += sext(imm, 12);
//        }
//        case BLT: {
//            if (Reg[rs1] < Reg[rs2]) PC += sext(imm, 12);
//        }
//        case BGE: {
//            if (Reg[rs1] >= Reg[rs2]) PC += sext(imm, 12);
//        }
//        case BLTU: {
//            if (unsigned(Reg[rs1]) < unsigned(Reg[rs2])) PC += sext(imm, 12);
//        }
//        case BGEU: {
//            if (unsigned(Reg[rs1]) >= unsigned(Reg[rs2])) PC += sext(imm, 12);
//        }
//        case LB: {
//            Reg[rd] = sext(Mem.read(Reg[rs1] + sext(imm, 12), 1), 8);
//        }
//        case LH: {
//            Reg[rd] = sext(Mem.read(Reg[rs1] + sext(imm, 12), 2), 16);
//        }
//        case LW: {
//            Reg[rd] = sext(Mem.read(Reg[rs1] + sext(imm, 12), 4), 32);
//        }
//        case LBU: {
//            Reg[rd] = Mem.read(Reg[rs1] + sext(imm, 12), 1);
//        }
//        case LHU: {
//            Reg[rd] = Mem.read(Reg[rs1] + sext(imm, 12), 2);
//        }
//        case SB: {
//
//        }
//        case SH: {
//        }
//        case SW: {
//        }
//        case ADDI: {
//            Reg[rd] = Reg[rs1] + sext(imm, 12);
//        }
//        case SLTI: {
//            Reg[rd] = Reg[rs1] < sext(imm, 12);
//        }
//        case SLTIU: {
//            Reg[rd] = unsigned(Reg[rs1]) < unsigned(sext(imm, 12));
//        }
//        case XORI: {
//            Reg[rd] = Reg[rs1] ^ sext(imm, 12);
//        }
//        case ORI: {
//            Reg[rd] = Reg[rs1] | sext(imm, 12);
//        }
//        case ANDI: {
//            Reg[rd] = Reg[rs1] & sext(imm, 12);
//        }
//        case SLLI: {
//            Reg[rd] = Reg[rs1] << rs2;
//        }
//        case SRLI: {
//            Reg[rd] = (unsigned)Reg[rs1] >> rs2;
//        }
//        case SRAI: {
//            Reg[rd] = Reg[rs1] >> rs2;
//        }
//        case ADD: {
//            Reg[rd] = Reg[rs1] + Reg[rs2];
//        }
//        case SUB: {
//            Reg[rd] = Reg[rs1] - Reg[rs2];
//        }
//        case SLL: {
//            Reg[rd]=Reg[rs1]<<(Reg[rs2]&~(-1<<5));
//        }
//        case SLT: {
//            Reg[rd]=Reg[rs1]<Reg[rs2];
//        }
//        case SLTU: {
//            Reg[rd]=unsigned (Reg[rs1])<unsigned (Reg[rs2]);
//        }
//        case XOR: {
//            Reg[rd]=Reg[rs1]^Reg[rs2];
//        }
//        case SRL: {
//            Reg[rd]=unsigned (Reg[rs1])>>(Reg[rs2]&~(-1<<5));
//        }
//        case SRA: {
//            Reg[rd]=Reg[rs1]>>(Reg[rs2]&~(-1<<5));
//        }
//        case OR: {
//            Reg[rd]=Reg[rs1]|Reg[rs2];
//        }
//        case AND: {
//            Reg[rd]=Reg[rs1]&Reg[rs2];
//        }
//    }
//}

void cpu::Fetch() {
    FTB.push();
}

void cpu::DecodeRenameDispatch() {
    UOP uop(FTB.front());
    uop.PC = PC;
    if (uop.type != B && uop.type != S) {
        uop.rd = Reg.reserve_S(uop.rd);
        if (uop.rd == PHYSICAL_REGISTER_MAX) return;
    }
    if (uop.type == R || uop.type == S || uop.type == B) {
        uop.rs1 = Reg.reserve_L(uop.rs1);
        uop.rs2 = Reg.reserve_L(uop.rs2);
    } else if (uop.type == I) {
        uop.rs1 = Reg.reserve_L(uop.rs1);
    }
    FTB.pop();
    if (RoB.full()) return;
    if (uop.type == S && LSU.storeFull()) return;
    else if ((uop.name == LB || uop.name == LBU || uop.name == LH || uop.name == LHU || uop.name == LW) &&
             LSU.loadFull())
        return;
    uop.RoB_pr = RoB.push(uop);
    if (uop.type == S)uop.LSU_pr = LSU.pushStore(uop.name, uop.rd);
    else if (uop.name == LB || uop.name == LBU || uop.name == LH || uop.name == LHU || uop.name == LW)
        uop.LSU_pr = LSU.pushLoad(uop.name, uop.rd);

}


//void cpu::Commit(){
//    if(!RoB.empty()&&RoB.ready())  {
//        ReorderBuffer::Reorder curReorder;
//        curReorder=RoB.front();
//        if(RoB.front().desType==1){}
//        else{
//            Reg[curReorder.des]=
//        }
//    }
//}