#ifndef RISCVCPUSIMULATOR_FUNCTIONALUNIT_HPP
#define RISCVCPUSIMULATOR_FUNCTIONALUNIT_HPP


const int ALU_Max = 10;

enum Operation_Type {
    Add, Sub, Or, And, Xor, Shift_Left, Shift_Right_Logical, Shift_Right_Arithmetic, Less_Than, Less_Than_Unsigned
};


class FunctionalUnit {
    friend class cpu;

private:
    class ALU {
    public:
        bool free_ = true;
        int input1_ = 0;
        int input2_ = 0;
        Operation_Type operation_type_ = Add;
        int RoB_pr_;

        ALU() = default;

        ALU(bool free, int input1, int input2, Operation_Type operation_type,int RoB_pr) :
                free_(free), input1_(input1), input2_(input2), operation_type_(operation_type),RoB_pr_(RoB_pr) {}

        int Execute() {
            if (operation_type_ == Add) return input1_ + input2_;
            else if (operation_type_ == Sub) return input1_ - input2_;
            else if (operation_type_ == And) return input1_ & input2_;
            else if (operation_type_ == Or) return input1_ | input2_;
            else if (operation_type_ == Xor)return input1_ ^ input2_;
            else if (operation_type_ == Shift_Left) return input1_ << input2_;
            else if (operation_type_ == Shift_Right_Logical) return input1_ >> input2_;
            else if (operation_type_ == Shift_Right_Arithmetic) return (unsigned) input1_ << input2_;
            else if (operation_type_ == Less_Than_Unsigned) return (unsigned) input1_ < (unsigned) input2_;
            else return input1_ < input2_;
        }
    };

    ALU units_[ALU_Max];
    int count;

    int Push(int input1, int input2, Operation_Type operation_type,int RoB_pr) {
        ++count;
        for (int i = 0; i < ALU_Max; ++i) {
            if (units_[i].free_) {
                units_[i] = {false, input1, input2, operation_type,RoB_pr};
                return i;
            }
        }
        return 0;
    }

    int Execute(int i) {
        units_[i].free_ = true;
        --count;
        return units_[i].Execute();
    }

    bool Full() { return count == ALU_Max; }

    ALU &operator[](int num){
        return units_[num];
    }

    void Clear(){
        for(int i=0;i<ALU_Max;++i) units_[i].free_=true;
    }
};

#endif //RISCVCPUSIMULATOR_FUNCTIONALUNIT_HPP
