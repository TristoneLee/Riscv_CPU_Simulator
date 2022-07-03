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
    Memory() {
        init();
    }

    void write(uint32_t loc, int len, uint32_t data) {
        switch (len) {
            case 1:
                mem[loc] = data & 255u;
                break;
            case 2:
                mem[loc] = data & 255u;
                mem[loc + 1] = ((data & 65535u) >> 8);
                break;
            case 4:
                mem[loc] = data & 255u;
                mem[loc + 1] = ((data & 65535u) >> 8);
                mem[loc + 2] = ((data & 16777216u) >> 16);
                mem[loc + 3] = data >> 24;
        }
    }

    uint32_t read(uint32_t loc, int len) {
        uint32_t ans = mem[loc + len - 1];
        for (int i = len - 2; i >= 0; --i) {
            ans <<= 8;
            ans += mem[loc + i];
        }
        return ans;
    }

    uint32_t readIns(uint32_t loc) {
        uint32_t ans = mem[loc + 3];
        for (int i = 2; i >= 0; --i) {
            ans <<=8;
            ans += mem[loc + i];
        }
        return ans;
    }
};

void Memory::init() {
    std::string s;
    uint32_t pos;
    while (cin >> s) {
        if (s[0] == '@') {
            std::stringstream sstring(s.substr(1));
            sstring >> hex >> pos;
        } else {
            if (s[0] >= '0' && s[0] <= '9') mem[pos] = (s[0] - '0') << 4;
            else mem[pos] = (s[0] - 'A'+10) << 4;
            if (s[1] >= '0' && s[1] <= '9') mem[pos] += (s[1] - '0');
            else mem[pos] += s[1] - 'A'+10;
            pos++;
        }
    }
}


#endif //RISCV_CPU_SIMULATOR_MEMORY_H
