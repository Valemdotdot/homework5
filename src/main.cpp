#include "memory.h"
#include "cpu.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>

// Lee un entero completo; admite decimal (100) y hexadecimal (0x64).
static uint32_t parseNumber(const std::string& value) {
    if (value.empty() || value[0] == '-') {
        throw std::invalid_argument("numero invalido");
    }
    size_t end = 0;
    int base = (value.size() > 2 && value[0] == '0' &&
                (value[1] == 'x' || value[1] == 'X')) ? 16 : 10;
    unsigned long long number = std::stoull(value, &end, base);
    if (end != value.size() || number > std::numeric_limits<uint32_t>::max()) {
        throw std::out_of_range("numero fuera del rango de 32 bits");
    }
    return static_cast<uint32_t>(number);
}

static void showHelp() {
    std::cout << "Comandos:\n"
              << "  pc                  Mostrar PC\n"
              << "  regs [xN ...]       Mostrar registros\n"
              << "  step                Ejecutar una instruccion\n"
              << "  run N               Ejecutar hasta N instrucciones\n"
              << "  mem DIRECCION N     Mostrar N bytes (1-256)\n"
              << "  reset               Reiniciar CPU y recargar programa\n"
              << "  help                Ver comandos\n"
              << "  exit                Salir\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: ./sim programa.bin\n";
        return 1;
    }

    const uint32_t MEMORY_SIZE = 1024 * 1024;
    Memory memory(MEMORY_SIZE);
    CPU cpu(memory);
    if (!memory.loadBinary(argv[1], 0)) return 1;
    std::cout << "Programa cargado correctamente. Escribe 'help' para ver comandos.\n";

    std::string line;
    while (true) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line)) break;
        std::istringstream input(line);
        std::string comando, arg, extra;
        if (!(input >> comando)) continue;

        try {
            if (comando == "exit") {
                std::cout << "Saliendo del simulador.\n";
                break;
            } else if (comando == "help") {
                showHelp();
            } else if (comando == "pc") {
                cpu.printPC();
            } else if (comando == "regs") {
                if (!(input >> arg)) {
                    cpu.printRegs();
                } else {
                    do {
                        if (arg.size() < 2 || arg[0] != 'x') {
                            throw std::invalid_argument("registro invalido: usa x0 a x31");
                        }
                        uint32_t index = parseNumber(arg.substr(1));
                        if (index > 31) throw std::out_of_range("registro fuera de x0-x31");
                        std::cout << "x" << std::setw(2) << std::setfill('0') << index
                                  << " = 0x" << std::hex << std::setw(8)
                                  << cpu.getReg(static_cast<int>(index)) << std::dec << "\n";
                    } while (input >> arg);
                }
            } else if (comando == "step") {
                cpu.step();
                std::cout << "Instruccion ejecutada.\n";
            } else if (comando == "run") {
                if (!(input >> arg) || (input >> extra)) {
                    std::cout << "Uso: run N\n";
                    continue;
                }
                uint32_t n = parseNumber(arg);
                if (n == 0 || n > 1000000u) {
                    std::cout << "N debe estar entre 1 y 1000000.\n";
                    continue;
                }
                uint32_t executed = 0;
                try {
                    for (; executed < n; ++executed) cpu.step();
                } catch (const std::exception& e) {
                    std::cout << "Error en PC=0x" << std::hex << std::setw(8)
                              << std::setfill('0') << cpu.getPC() << std::dec
                              << ": " << e.what() << "\n";
                }
                std::cout << "Se ejecutaron " << executed << " instrucciones.\n";
            } else if (comando == "mem") {
                std::string countText;
                if (!(input >> arg >> countText) || (input >> extra)) {
                    std::cout << "Uso: mem DIRECCION CANTIDAD\n";
                    continue;
                }
                uint32_t address = parseNumber(arg);
                uint32_t count = parseNumber(countText);
                if (count == 0 || count > 256) {
                    std::cout << "Cantidad invalida (1-256).\n";
                    continue;
                }
                if (address >= MEMORY_SIZE || count > MEMORY_SIZE - address) {
                    throw std::out_of_range("lectura fuera de memoria");
                }
                for (uint32_t i = 0; i < count; ++i) {
                    if (i % 8 == 0) {
                        if (i) std::cout << "\n";
                        std::cout << "0x" << std::hex << std::setw(8)
                                  << std::setfill('0') << (address + i) << ": ";
                    }
                    std::cout << std::hex << std::setw(2) << std::setfill('0')
                              << static_cast<unsigned>(memory.read8(address + i)) << " ";
                }
                std::cout << std::dec << "\n";
            } else if (comando == "reset") {
                memory.clear();
                if (!memory.loadBinary(argv[1], 0)) {
                    std::cerr << "No se pudo recargar el programa.\n";
                    return 1;
                }
                cpu.reset();
                std::cout << "CPU y memoria reiniciadas.\n";
            } else {
                std::cout << "Comando no reconocido. Escribe help.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
    return 0;
}