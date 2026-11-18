
#ifndef DECODER_H
#define DECODER_H

#include <cstdint>

struct DecodedInstruction {
    uint32_t instruction;

    uint8_t opcode;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;

    uint8_t funct3;
    uint8_t funct7;

    int32_t imm;
};

DecodedInstruction decode(uint32_t instruction);

#endif