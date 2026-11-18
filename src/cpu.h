
#ifndef CPU_H
#define CPU_H

#include "memory.h"
#include <cstdint>

class CPU {
private:
    uint32_t pc;
    uint32_t regs[32];
    Memory& memory;

public:
    CPU(Memory& mem);

    void reset();

    uint32_t getPC() const;
    uint32_t getReg(int index) const;

    uint32_t fetch() const;

    void printPC() const;
    void printRegs() const;
    void step();
};

#endif