#include "cpu.h"
#include "decoder.h"
#include <iostream>
#include <iomanip>


CPU::CPU(Memory& mem) : memory(mem) {
    reset();
}

void CPU::reset() {
    pc = 0;

    for (int i = 0; i < 32; i++) {
        regs[i] = 0;
    }
}


uint32_t CPU::getPC() const {
    return pc;
}

uint32_t CPU::getReg(int index) const {
    if (index < 0 || index >= 32) {
        return 0;
    }

    return regs[index];
}


uint32_t CPU::fetch() const {
    return memory.read32(pc);
}


void CPU::printPC() const {
    std::cout << "PC = 0x"
              << std::hex << std::setw(8) << std::setfill('0')
              << pc << std::dec << "\n";
}


void CPU::printRegs() const {
    for (int i = 0; i < 32; i++) {
        std::cout << "x" << std::setw(2) << std::setfill('0') << i
                  << " = 0x"
                  << std::hex << std::setw(8) << std::setfill('0')
                  << regs[i]
                  << std::dec << "\n";
    }
}

void CPU::step() {
    uint32_t instruction = fetch();
    DecodedInstruction d = decode(instruction);
}
