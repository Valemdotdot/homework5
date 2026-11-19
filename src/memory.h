#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <vector>
#include <string>

class Memory {
private:
    std::vector<uint8_t> data;

public:
    explicit Memory(uint32_t size);

    void clear();
    bool loadBinary(const std::string& filename, uint32_t startAddress);

    uint8_t read8(uint32_t address) const;
    uint16_t read16(uint32_t address) const;
    uint32_t read32(uint32_t address) const;

    void write8(uint32_t address, uint8_t value);
    void write16(uint32_t address, uint16_t value);
    void write32(uint32_t address, uint32_t value);
};

#endif