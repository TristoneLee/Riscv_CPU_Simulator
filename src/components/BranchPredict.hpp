#ifndef SIMULATOR_BRANCHPREDICT_HPP
#define SIMULATOR_BRANCHPREDICT_HPP

class Predictor{
    friend class cpu;
private:
    struct 2Bit_Predictor{
        int value=1;

        void Plus(){
            if(value<3) value++;
        }

        void Minus(){
            if(value) value--;
        }

        bool Predictor(){
            if(value>=2) return true;
        }
    }
};


#endif //SIMULATOR_BRANCHPREDICT_HPP
