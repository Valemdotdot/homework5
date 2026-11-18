
#include "memory.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

Memory::Memory(uint32_t size) {
    data.resize(size, 0);
}

bool Memory::loadBinary(const std::string& filename, uint32_t startAddress) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: no se pudo abrir el archivo " << filename << "\n";
        return false;
    }

    uint32_t address = startAddress;
    char byte;

    while (file.get(byte)) {
        if (address >= data.size()) {
            std::cerr << "Error: el programa excede la memoria disponible\n";
            return false;
        }

        data[address] = static_cast<uint8_t>(byte);
        address++;
    }

    return true;
}

uint8_t Memory::read8(uint32_t address) const {
    if (address >= data.size()) {
        throw std::out_of_range("Lectura fuera de memoria");
    }

    return data[address];
}


uint16_t Memory::read16(uint32_t address) const {
    return static_cast<uint16_t>(read8(address)) |
           (static_cast<uint16_t>(read8(address + 1)) << 8);
}


uint32_t Memory::read32(uint32_t address) const {
    return static_cast<uint32_t>(read8(address)) |
           (static_cast<uint32_t>(read8(address + 1)) << 8) |
           (static_cast<uint32_t>(read8(address + 2)) << 16) |
           (static_cast<uint32_t>(read8(address + 3)) << 24);
}


void Memory::write8(uint32_t address, uint8_t value) {
    if (address >= data.size()) {
        throw std::out_of_range("Escritura fuera de memoria");
    }

    data[address] = value;
}


void Memory::write16(uint32_t address, uint16_t value) {
    write8(address, value & 0xFF);
    write8(address + 1, (value >> 8) & 0xFF);
}


void Memory::write32(uint32_t address, uint32_t value) {
    write8(address, value & 0xFF);
    write8(address + 1, (value >> 8) & 0xFF);
    write8(address + 2, (value >> 16) & 0xFF);
    write8(address + 3, (value >> 24) & 0xFF);
}

