#ifndef RISCVCPUSIMULATOR_ISSUEQUEUE_H
#define RISCVCPUSIMULATOR_ISSUEQUEUE_H

#include "tools.h"
#include "decoder.h"


const int Issue_QUEUE_MAX=100;
const int INTERGER_ISSUE_QUEUE_MAX=100;
const int MEMORY_ISSUE_QUEUE_MAX=100;

class IssueQueue{
    friend class cpu;
private:
    struct IssueUnit{
        insName name;
        bool ready=false;
        int Q1=0;
        int Q2=0;
        int A=0;  //immediate
        bool R1=false;
        bool R2=false;
    };
};

#endif //RISCVCPUSIMULATOR_ISSUEQUEUE_H
