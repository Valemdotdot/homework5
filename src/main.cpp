
#include "memory.h"
#include "cpu.h"
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Uso: ./sim programa.bin\n";
        return 1;
    }

    const uint32_t MEMORY_SIZE = 1024 * 1024;

    Memory memory(MEMORY_SIZE);
    CPU cpu(memory);

    if (!memory.loadBinary(argv[1], 0x00000000)) {
        return 1;
    }

    std::cout << "Programa cargado correctamente.\n";

    cpu.printPC();
    cpu.printRegs();
    cpu.step();

    cpu.printPC();
    cpu.printRegs();

    uint32_t instr = cpu.fetch();

    std::cout << "Instruccion en PC = 0x"
              << std::hex << std::setw(8) << std::setfill('0')
              << instr << std::dec << "\n";

    return 0;
}