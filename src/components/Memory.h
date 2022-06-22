#ifndef RISCV_CPU_SIMULATOR_MEMORY_H
#define RISCV_CPU_SIMULATOR_MEMORY_H

#include <sstream>
#include <cstring>
#include <iostream>

using namespace std;

class Memory {
private:
    uint8_t mem[500000];
    void init();

public:
    Memory(){
        init();
    }

    void write(uint8_t loc, int len,uint8_t *data){
    }

    void read(uint8_t loc, int len, uint8_t *des){
        for(int i=0;i<len;++i) des[i]=mem[loc+len-i-1];
    }

    uint8_t *read(uint32_t loc, int len){
        uint8_t des[len];
        for(int i=0;i<len;++i) des[i]=mem[loc+len-i-1];
    }

    uint32_t readIns(uint32_t loc){
        uint32_t ans=mem[loc+3];
        for(int i=2;i>=0;--i){
            ans<<8;
            ans+=mem[loc+i];
        }
        return ans;
    }
};

void Memory::init() {
    std::string s;
    uint32_t pos;
    while(cin>>s) {
        if (s[0] == '@') {
            std::stringstream sstring(s.substr(1));
            sstring >> hex >> pos;
        } else {
            if(s[0]>='0'&&s[0]<='9') mem[pos]=(s[0]-'0')<<4;
            else mem[pos]=(s[0]-'a')<<4;
            if(s[1]>='0'&&s[1]<='0') mem[pos]+=(s[1]-'0');
            else mem[pos]+=s[1]-'a';
            pos++;
        }
    }
}


#endif //RISCV_CPU_SIMULATOR_MEMORY_H
