#ifndef RISCVCPUSIMULATOR_DECODER_H
#define RISCVCPUSIMULATOR_DECODER_H


//Register, Immediate, Save, Branch, Upper Immediate, Jump
enum InsType {
    R, I, S, B, U, J
};

struct baseIns {
    uint32_t ins;
    InsType type;
    uint8_t opcode = 0;
    uint8_t rd = 0;
    uint8_t rs1 = 0;
    uint8_t funct3 = 0;
    uint8_t rs2 = 0;
    uint8_t funct7 = 0;

    InsType typeSelect() {
        if (opcode == 51) return R;
        else if (opcode == 19) {
            if (funct3 == 1 || funct3 == 5) return R;
            else return I;
        } else if (opcode == 35) return S;
        else if (opcode == 3) return I;
        else if (opcode == 99) return B;
        else if (opcode == 103 || opcode == 111) return J;
        else if (opcode == 23 || opcode == 55) return U;
    }

    baseIns() = default;

    baseIns(uint32_t source) : ins(source) {
        opcode = ins & 127u;
        rd = (ins >> 7) & 31u;
        funct3 = (ins >> 12) & 7u;
        rs1 = (ins >> 15) & 31u;
        rs2 = (ins >> 18) & 31u;
        funct7 = (ins >> 25) & 127u;
        type = typeSelect();
    }
};

enum InsName {
    LUI,
    AUIPC,
    JAL,
    JALR,
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,
    LB,
    LH,
    LW,
    LBU,
    LHU,
    SB,
    SH,
    SW,
    ADDI,
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,
    SRLI,
    SRAI,
    ADD,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND
};

class UOP {
    friend class cpu;
    friend class ReorderBuffer;


    InsType type;
    InsName name;
    uint8_t rd = 0;
    uint8_t rs1 = 0;
    uint8_t rs2 = 0;
    uint32_t  imm= 0;
    int PC=0;
    int RoB_pr=0;
    int LSU_pr=0;

public:
    UOP(baseIns base) : rd(base.rd), rs1(base.rs1), rs2(base.rs2), type(base.type) {
        uint8_t funct7 = base.funct7, funct3 = base.funct3, opcode = base.opcode;
        if (type == R) {
            if (funct7 == 0 && funct3 == 7) name = AND;
            else if (funct7 == 0 && funct3 == 6) name = OR;
            else if (funct7 == 64 && funct3 == 5)name = SRA;
            else if (funct7 == 0 && funct3 == 5)name = SRL;
            else if (funct7 == 0 && funct3 == 4) name = XOR;
            else if (funct7 == 0 && funct3 == 3) name = SLTU;
            else if (funct7 == 0 && funct3 == 2) name = SLT;
            else if (funct7 == 0 && funct3 == 1) name = SLL;
            else if (funct7 == 64 && funct3 == 0) name = SUB;
            else if (funct7 == 0 && funct3 == 0) name = ADD;
            else if (funct7 == 64 && funct3 == 5) name = SRAI;
            else if (funct7 == 0 && funct3 == 5) name = SRLI;
            else if (funct7 == 0 && funct3 == 1) name = SLLI;
        } else if (type == I) {
            if (opcode == 19) {
                if (funct3 == 7) name = ANDI;
                else if (funct3 == 6) name = ORI;
                else if (funct3 == 4) name = XORI;
                else if (funct3 == 3) name = SLTIU;
                else if (funct3 == 2) name = SLTI;
                else if (funct3 == 0) name = ADDI;
            } else if (opcode == 3) {
                if (funct3 == 5) name = LHU;
                else if (funct3 == 4) name = LBU;
                else if (funct3 == 2) name = LW;
                else if (funct3 == 1) name = LH;
                else if (funct3 == 0) name = LB;
            }
            if (base.funct7 >> 10 == 1)imm = ~(base.funct7 - 1);
            else imm = base.funct7;
        } else if (type == S) {
            if (funct3 == 0) name = SB;
            else if (funct3 == 1) name = SH;
            else if (funct3 == 2) name = SW;
            imm = ((uint32_t) funct7 << 4) + (rd);
            if (imm >> 6 == 1)imm = ~(base.funct7 - 1);
            else imm = base.funct7;
        } else if (type == B) {
            if (funct3 == 0) name = BEQ;
            else if (funct3 == 1) name = BNE;
            else if (funct3 == 4) name = BLT;
            else if (funct3 == 5) name = BGE;
            else if (funct3 == 6) name = BLTU;
            else if (funct3 == 7) name = BGEU;
            imm = 1;
        } else if (type == U) {
            if (opcode == 55) name = LUI;
            else if (opcode == 23) name = AUIPC;
            imm = 1;
        } else if (type == J) {
            if (opcode == 111) name = JAL;
            else if (opcode == 103) name = JALR;
            imm = 1;
        }
    }
};


#endif //RISCVCPUSIMULATOR_DECODER_H
