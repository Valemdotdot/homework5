#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

constexpr size_t MEMORY_SIZE = 0x10000;

class Simulator {
private:
    bool halted = false;
    uint16_t pc = 0;
    uint16_t regs[4] = {};
    std::vector<uint16_t> memory;

public:
    Simulator()
        : memory(MEMORY_SIZE) {}

    bool load_from_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open())
            return false;

        file.read((char*)memory.data(), MEMORY_SIZE * sizeof(memory[0]));
        return true;
    }

    bool is_halted() const {
        return halted;
    }

    void step() {
        uint16_t instr = memory[pc];
        uint8_t opcode = (instr >> 12) & 0xF;  // Bits 15:12
        uint8_t rd = (instr >> 10) & 0x3;      // Bits 11:10
        uint8_t rs1 = (instr >> 8) & 0x3;      // Bits 9:8
        uint8_t rs2 = (instr >> 6) & 0x3;      // Bits 7:6
        uint8_t imm = instr & 0x3F;            // Bits 5:0

        pc++;

        switch (opcode) {
            case 0x0:  // nop
                break;
            case 0x1:  // mvi rd, imm
                regs[rd] = imm;
                break;
            case 0x2:  // ld rd,(rs1)
                regs[rd] = memory[regs[rs1]];
                break;
            case 0x3:  // st rs2,(rs1)
                memory[regs[rs1]] = regs[rs2];
                break;
            case 0x4:  // jmp imm
                pc += (int8_t)imm;
                break;
            case 0x5:  // jnz rs1, imm
                if (regs[rs1] != 0)
                    pc += (int8_t)imm;
                break;
            case 0x6:  // jz rs1, imm
                if (regs[rs1] == 0)
                    pc += (int8_t)imm;
                break;
            case 0x8:  // add rd, rs1, rs2
                regs[rd] = regs[rs1] + regs[rs2];
                break;
            case 0x9:  // sub rd, rs1, rs2
                regs[rd] = regs[rs1] - regs[rs2];
                break;
            case 0xA:  // and rd, rs1, rs2
                regs[rd] = regs[rs1] & regs[rs2];
                break;
            case 0xB:  // or rd, rs1, rs2
                regs[rd] = regs[rs1] | regs[rs2];
                break;
            case 0xC:  // mov rd, rs1
                regs[rd] = regs[rs1];
                break;
            case 0xD:  // in rd
                std::cin >> regs[rd];
                break;
            case 0xE:  // out rs2
                std::cout << regs[rs2] << "\n";
                break;
            case 0xF:  // halt
                halted = true;
                pc--;
                break;
        }
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Debes especificar un archivo de entrada.\n";
        return 1;
    }

    Simulator sim{};

    if (!sim.load_from_file(argv[1])) {
        std::cerr << "No se pudo cargar el archivo.\n";
        return 1;
    }

    while (!sim.is_halted())
        sim.step();

    return 0;
}
