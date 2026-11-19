#include "decoder.h"

// Extiende imm signed a 32 bits
static int32_t signExtend(uint32_t value, unsigned bits) {
    uint32_t sign = 1u << (bits - 1);
    return static_cast<int32_t>((value ^ sign) - sign);
}

DecodedInstruction decode(uint32_t instruction) {
    DecodedInstruction d{};

    d.instruction = instruction;
    d.opcode = instruction & 0x7F;
    d.rd = (instruction >> 7) & 0x1F;
    d.funct3 = (instruction >> 12) & 0x07;
    d.rs1 = (instruction >> 15) & 0x1F;
    d.rs2 = (instruction >> 20) & 0x1F;
    d.funct7 = (instruction >> 25) & 0x7F;

    switch (d.opcode) {

        case 0x03: // LOAD
        case 0x13: // Operaciones inmediatas
        case 0x67: // JALR
            // Tipo I
            d.imm = signExtend(instruction >> 20, 12);
            break;

        case 0x23: {
            // Tipo S: STORE
            uint32_t imm =
                ((instruction >> 25) << 5) |
                ((instruction >> 7) & 0x1F);

            d.imm = signExtend(imm, 12);
            break;
        }

        case 0x63: {
            // Tipo B: Branches
            uint32_t imm =
                (((instruction >> 31) & 1u) << 12) |
                (((instruction >> 7) & 1u) << 11) |
                (((instruction >> 25) & 0x3F) << 5) |
                (((instruction >> 8) & 0xF) << 1);

            d.imm = signExtend(imm, 13);
            break;
        }

        case 0x37: // LUI
        case 0x17: // AUIPC
            // Tipo U
            d.imm = static_cast<int32_t>(
                instruction & 0xFFFFF000u
            );
            break;

        case 0x6F: {
            // Tipo J: JAL
            uint32_t imm =
                (((instruction >> 31) & 1u) << 20) |
                (((instruction >> 12) & 0xFF) << 12) |
                (((instruction >> 20) & 1u) << 11) |
                (((instruction >> 21) & 0x3FF) << 1);

            d.imm = signExtend(imm, 21);
            break;
        }

        default:
            d.imm = 0;
            break;
    }

    return d;
}