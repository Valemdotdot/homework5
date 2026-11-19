#include "cpu.h"
#include "decoder.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <sstream>


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
    if (pc & 3u) {
        throw std::runtime_error("PC desalineado (RV32I requiere instrucciones de 4 bytes)");
    }
    uint32_t instruction = fetch();
    DecodedInstruction d = decode(instruction);

    executeInstruction(d);

    regs[0] = 0;
}



void CPU::executeInstruction(const DecodedInstruction& d) {
    if (d.opcode == 0x13) {
        executeIType(d);
    } else if (d.opcode == 0x33) {
        executeRType(d);
    } else if (d.opcode == 0x03) {
        executeLoadType(d);
    } else if (d.opcode == 0x23) {
        executeStoreType(d);
    } else if (d.opcode == 0x63) {
        executeBType(d);
    } else if (d.opcode == 0x6F) {
        executeJType(d);
    } else if (d.opcode == 0x67) {
        executeJALR(d);
    } else if (d.opcode == 0x37 || d.opcode == 0x17) {
        executeUType(d);
    } else {
        std::ostringstream err;
        err << "Instruccion invalida/no soportada: 0x"
            << std::hex << d.instruction;
        throw std::runtime_error(err.str());
    }
}


void CPU::executeIType(const DecodedInstruction& d) {
    uint32_t a = regs[d.rs1];
    uint32_t imm = static_cast<uint32_t>(d.imm);
    uint32_t shamt = (d.instruction >> 20) & 0x1F;

    switch (d.funct3) {
        case 0x0: // ADDI
            regs[d.rd] = a + imm;
            break;

        case 0x7: // ANDI
            regs[d.rd] = a & imm;
            break;

        case 0x6: // ORI
            regs[d.rd] = a | imm;
            break;

        case 0x4: // XORI
            regs[d.rd] = a ^ imm;
            break;

        case 0x2: // SLTI
            regs[d.rd] = (static_cast<int32_t>(a) < d.imm) ? 1 : 0;
            break;

        case 0x3: // SLTIU
            regs[d.rd] = (a < imm) ? 1 : 0;
            break;

        case 0x1: // SLLI
            if (d.funct7 != 0x00) {
                throw std::runtime_error("SLLI invalida");
            }
            regs[d.rd] = a << shamt;
            break;

        case 0x5:
            if (d.funct7 == 0x00) {
                // SRLI
                regs[d.rd] = a >> shamt;
            } else if (d.funct7 == 0x20) {
                // SRAI
                if (shamt == 0) {
                    regs[d.rd] = a;
                } else {
                    uint32_t result = a >> shamt;
                    if (a & 0x80000000u)
                        result |= (~0u << (32 - shamt));
                    regs[d.rd] = result;
                }
            } else {
                throw std::runtime_error("Shift inmediato invalido");
            }
            break;

        default:
            throw std::runtime_error("Instruccion Tipo I invalida");
    }
    pc += 4;
}


void CPU::executeRType(const DecodedInstruction& d) {
    uint32_t a = regs[d.rs1];
    uint32_t b = regs[d.rs2];
    uint32_t shamt = b & 0x1F;
    uint32_t result = 0;

    switch (d.funct3) {
        case 0x0:
            if (d.funct7 == 0x00)
                result = a + b; // ADD
            else if (d.funct7 == 0x20)
                result = a - b; // SUB
            else
                throw std::runtime_error("Operacion R invalida");
            break;

        case 0x7: // AND
            if (d.funct7 != 0x00) throw std::runtime_error("funct7 invalido en R-Type");
            result = a & b;
            break;

        case 0x6: // OR
            if (d.funct7 != 0x00) throw std::runtime_error("funct7 invalido en R-Type");
            result = a | b;
            break;

        case 0x4: // XOR
            if (d.funct7 != 0x00) throw std::runtime_error("funct7 invalido en R-Type");
            result = a ^ b;
            break;

        case 0x2: // SLT
            if (d.funct7 != 0x00) throw std::runtime_error("funct7 invalido en R-Type");
            result = (static_cast<int32_t>(a) <
                      static_cast<int32_t>(b)) ? 1 : 0;
            break;

        case 0x3: // SLTU
            if (d.funct7 != 0x00) throw std::runtime_error("funct7 invalido en R-Type");
            result = (a < b) ? 1 : 0;
            break;

        case 0x1: // SLL
            if (d.funct7 != 0x00) throw std::runtime_error("funct7 invalido en R-Type");
            result = a << shamt;
            break;

        case 0x5:
            if (d.funct7 == 0x00) {
                result = a >> shamt; // SRL
            } else if (d.funct7 == 0x20) {
                // SRA: desplazamiento aritmetico
                if (shamt == 0) {
                    result = a;
                } else {
                    result = a >> shamt;
                    if (a & 0x80000000u)
                        result |= (~0u << (32 - shamt));
                }
            } else {
                throw std::runtime_error("Shift R invalido");
            }
            break;

        default:
            throw std::runtime_error("funct3 invalido en R-Type");
    }

    regs[d.rd] = result;
    pc += 4;
}


void CPU::executeLoadType(const DecodedInstruction& d) {
    uint32_t address = regs[d.rs1] +
                       static_cast<uint32_t>(d.imm);

    uint32_t result = 0;

    switch (d.funct3) {
        case 0x0: // LB
            result = static_cast<int32_t>(
                static_cast<int8_t>(memory.read8(address)));
            break;

        case 0x1: // LH
            result = static_cast<int32_t>(
                static_cast<int16_t>(memory.read16(address)));
            break;

        case 0x2: // LW
            result = memory.read32(address);
            break;

        case 0x4: // LBU
            result = memory.read8(address);
            break;

        case 0x5: // LHU
            result = memory.read16(address);
            break;

        default:
            throw std::runtime_error("LOAD invalida");
    }

    regs[d.rd] = result;
    pc += 4;
}

void CPU::executeStoreType(const DecodedInstruction& d) {
    uint32_t address = regs[d.rs1] +
                       static_cast<uint32_t>(d.imm);

    uint32_t value = regs[d.rs2];

    switch (d.funct3) {
        case 0x0: // SB
            memory.write8(address, static_cast<uint8_t>(value));
            break;

        case 0x1: // SH
            memory.write16(address, static_cast<uint16_t>(value));
            break;

        case 0x2: // SW
            memory.write32(address, value);
            break;

        default:
            throw std::runtime_error("STORE invalida");
    }

    pc += 4;
}


void CPU::executeBType(const DecodedInstruction& d) {
    uint32_t a = regs[d.rs1];
    uint32_t b = regs[d.rs2];

    bool taken = false;

    switch (d.funct3) {
        case 0x0: // BEQ
            taken = (a == b);
            break;

        case 0x1: // BNE
            taken = (a != b);
            break;

        case 0x4: // BLT
            taken = (static_cast<int32_t>(a) <
                     static_cast<int32_t>(b));
            break;

        case 0x5: // BGE
            taken = (static_cast<int32_t>(a) >=
                     static_cast<int32_t>(b));
            break;

        case 0x6: // BLTU
            taken = (a < b);
            break;

        case 0x7: // BGEU
            taken = (a >= b);
            break;

        default:
            throw std::runtime_error("Branch invalido");
    }

    if (taken) {
        pc += static_cast<uint32_t>(d.imm);
    } else {
        pc += 4;
    }
}

void CPU::executeJType(const DecodedInstruction& d) {
    // JAL
    uint32_t returnAddress = pc + 4;
    uint32_t target = pc + static_cast<uint32_t>(d.imm);

    regs[d.rd] = returnAddress;
    pc = target;
}

void CPU::executeJALR(const DecodedInstruction& d) {
    // JALR
    if (d.funct3 != 0x0) {
        throw std::runtime_error("JALR invalida");
    }

    uint32_t returnAddress = pc + 4;
    uint32_t target = (regs[d.rs1] +
                       static_cast<uint32_t>(d.imm)) & ~1u;

    regs[d.rd] = returnAddress;
    pc = target;
}

void CPU::executeUType(const DecodedInstruction& d) {
    uint32_t imm = static_cast<uint32_t>(d.imm);

    if (d.opcode == 0x37) {
        // LUI
        regs[d.rd] = imm;
    } else if (d.opcode == 0x17) {
        // AUIPC
        regs[d.rd] = pc + imm;
    } else {
        throw std::runtime_error("U-Type invalida");
    }

    pc += 4;
}