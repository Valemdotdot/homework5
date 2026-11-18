#include "decoder.h"

DecodedInstruction decode(uint32_t instruction) {
    DecodedInstruction d{};

    d.instruction = instruction;

    d.opcode = instruction & 0x7F;
    d.rd     = (instruction >> 7)  & 0x1F;
    d.funct3 = (instruction >> 12) & 0x07;
    d.rs1    = (instruction >> 15) & 0x1F;
    d.rs2    = (instruction >> 20) & 0x1F;
    d.funct7 = (instruction >> 25) & 0x7F;

    // Por ahora solo inmediato tipo I: addi, lw, jalr, etc.
    d.imm = static_cast<int32_t>(instruction) >> 20;

    return d;
}