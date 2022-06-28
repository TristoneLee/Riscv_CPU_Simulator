#ifndef RISCVCPUSIMULATOR_LOADSTOREUNIT_H
#define RISCVCPUSIMULATOR_LOADSTOREUNIT_H

#include "tools.h"
#include "decoder.h"

const int STORE_UNIT_MAX = 10;
const int LOAD_UNIT_MAX = 10;


class LoadStoreUnit {
    struct StoreUnit {
        insName name;
        int regLoc;
        bool ready = 0;
        int des = 0;
        int data = 0;

        StoreUnit(insName _name, int _regLoc, bool _ready = 0, int _des = 0, int _data = 0) :
                name(_name), regLoc(_regLoc), ready(_ready), des(_des), data(_data) {};
    };

    struct LoadUnit {
        insName name;
        int regLoc;
        bool ready = 0;
        int des = 0;
        int data = 0;

        LoadUnit(insName _name, int _regLoc, bool _ready = 0, int _des = 0, int _data = 0) :
                name(_name), regLoc(_regLoc), ready(_ready), des(_des), data(_data) {};
    };

    CircularQueue<LoadUnit, LOAD_UNIT_MAX> LDQ; //Load queue
    CircularQueue<StoreUnit, STORE_UNIT_MAX> STQ; //Store Address queue

public:
    int pushStore(insName _name, int _Regloc) { STQ.push({_name, _Regloc}); }

    int pushLoad(insName _name, int _Regloc) { return LDQ.push({_name, _Regloc}); }

    bool storeFull() { return STQ.full(); }

    bool loadFull() { return LDQ.full(); }
};

#endif //RISCVCPUSIMULATOR_LOADSTOREUNIT_H
