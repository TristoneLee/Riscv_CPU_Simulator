#ifndef RISCVCPUSIMULATOR_DECODER_HPP
#define RISCVCPUSIMULATOR_DECODER_HPP


//Register, Immediate, Save, Branch, Upper Immediate, Jump
enum InsType {
    R, I, S, B, U, J
};

struct BaseIns {
    uint32_t ins;
    InsType type;
    uint32_t opcode = 0;
    uint32_t rd = 0;
    uint32_t rs1 = 0;
    uint32_t funct3 = 0;
    uint32_t rs2 = 0;
    uint32_t funct7 = 0;

    InsType typeSelect() {
        if (opcode == 51) return R;
        else if (opcode == 19) {
            if (funct3 == 1 || funct3 == 5) return R;
            else return I;
        } else if (opcode == 35) return S;
        else if (opcode == 3 || opcode == 103) return I;
        else if (opcode == 99) return B;
        else if (opcode == 111) return J;
        else if (opcode == 23 || opcode == 55) return U;
    }

    BaseIns() = default;

    BaseIns(uint32_t source) : ins(source) {
        opcode = ins & 127u;
        rd = (ins >> 7) & 31u;
        funct3 = (ins >> 12) & 7u;
        rs1 = (ins >> 15) & 31u;
        rs2 = (ins >> 20) & 31u;
        funct7 = (ins >> 25) & 127u;
        type = typeSelect();
    }
};

enum InsName {
    NONE, LUI, AUIPC, JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU, LB, LH, LW, LBU, LHU, SB, SH, SW, ADDI, SLTI, SLTIU,
    XORI, ORI, ANDI, SLLI, SRLI, SRAI, ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND, RET, END
};

class UOP {
    friend class cpu;

    friend class ReorderBuffer;

    friend class IssueQueue;

    friend class LoadStoreUnit;

    InsType type = R;
    InsName name = NONE;
    uint32_t rd = 0;
    uint32_t rs1 = 0;
    uint32_t rs2 = 0;
    int imm = 0;
    int PC = 0;
    int RoB_pr = 0;
    int LSU_pr = 0;

public:
    UOP() = default;

    UOP(BaseIns base) : rd(base.rd), rs1(base.rs1), rs2(base.rs2), type(base.type) {
        uint8_t funct7 = base.funct7, funct3 = base.funct3, opcode = base.opcode;
        if (type == R) {
            if (funct7 == 0 && funct3 == 7) name = AND;
            else if (funct7 == 0 && funct3 == 6) name = OR;
            else if (funct7 == 32 && funct3 == 5)name = SRA;
            else if (funct7 == 0 && funct3 == 5)name = SRL;
            else if (funct7 == 0 && funct3 == 4) name = XOR;
            else if (funct7 == 0 && funct3 == 3) name = SLTU;
            else if (funct7 == 0 && funct3 == 2) name = SLT;
            else if (funct7 == 0 && funct3 == 1) name = SLL;
            else if (funct7 == 32 && funct3 == 0) name = SUB;
            else if (funct7 == 0 && funct3 == 0) name = ADD;
            else if (funct7 == 32 && funct3 == 5) name = SRAI;
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
            } else if (opcode == 103)
                if (rs1 == 1) {
                    name = RET;
                    return;
                } else {
                    name = JALR;
                    if (rd == 0) rd = 1;
                }
            imm = sext(rs2 + (funct7 << 5), 12);
            if (imm == 255 & name == ADDI) name = END;
        } else if (type == S) {
            if (funct3 == 0) name = SB;
            else if (funct3 == 1) name = SH;
            else if (funct3 == 2) name = SW;
            imm = (funct7 << 5) + rd;
            imm = sext(imm, 12);
        } else if (type == B) {
            if (funct3 == 0) name = BEQ;
            else if (funct3 == 1) name = BNE;
            else if (funct3 == 4) name = BLT;
            else if (funct3 == 5) name = BGE;
            else if (funct3 == 6) name = BLTU;
            else if (funct3 == 7) name = BGEU;
            imm = ((rd >> 1) << 1) + ((rd & 1) << 11) + ((funct7 & 64u) << 6) + ((funct7 & 63u) << 5);
            imm = sext(imm, 13);
        } else if (type == U) {
            if (opcode == 55) name = LUI;
            else if (opcode == 23) name = AUIPC;
            imm = (funct3 + (rs1 << 3) + (rs2 << 8) + (funct7 << 13)) << 12;
        } else if (type == J) {
            if (opcode == 111) {
                name = JAL;
                imm = (funct3 << 12) + (rs1 << 15) + ((rs2 >> 1) << 1) + ((rs2 & 1u) << 11) + ((funct7 & 63) << 5) +
                      ((funct7 & 64u) << 14);
            }
            imm = sext(imm, 21);
        }
    }
};


#endif //RISCVCPUSIMULATOR_DECODER_HPP
