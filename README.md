# Toy Simulator

Simulador para un ISA ficticio de 16 bits implementado en C++.

## Especificación del ISA

Este es un ISA de 16 bits, word-addressable.

- **Addressing**: Word-addressable.
- **Registros**: 4 registros de propósito general de 16 bits: `r0`, `r1`, `r2`, `r3`.

### Formato de instrucción

Cada instrucción es un word (16 bits) con el siguiente formato:

```
Bits:  15-12 | 11-10 | 9-8  | 7-6  | 5-0
       opcode   rd     rs1    rs2    imm
```

Los campos `rd`, `rs1`, `rs2` son índices a registros (2 bits, rango 0–3).

El campo `imm` se usa como valor inmediato sin signo. En saltos (`jmp`, `jnz`, `jz`) se interpreta con signo para realizar saltos relativos hacia adelante y atrás (+-32 de rango).

### Tabla de opcodes

| Opcode | Mnemónico          | Operación                          |
| ------ | ------------------ | ---------------------------------- |
| `0x0`  | `nop`              | Ninguna.                           |
| `0x1`  | `mvi rd, imm`      | `rd = imm`                         |
| `0x2`  | `ld rd, (rs1)`     | `rd = memory[rs1]`                 |
| `0x3`  | `st rs2, (rs1)`    | `memory[rs1] = rs2`                |
| `0x4`  | `jmp imm`          | `pc = pc + (i8)imm`                |
| `0x5`  | `jnz rs1, imm`     | `if (rs1 != 0) pc = pc + (i8)imm`  |
| `0x6`  | `jz rs1, imm`      | `if (rs1 == 0) pc = pc + (i8)imm`  |
| `0x7`  | -                  | _No implementado._                 |
| `0x8`  | `add rd, rs1, rs2` | `rd = rs1 + rs2`                   |
| `0x9`  | `sub rd, rs1, rs2` | `rd = rs1 - rs2`                   |
| `0xA`  | `and rd, rs1, rs2` | `rd = rs1 & rs2`                   |
| `0xB`  | `or rd, rs1, rs2`  | `rd = rs1 \| rs2`                  |
| `0xC`  | `mov rd, rs1`      | `rd = rs1`                         |
| `0xD`  | `in rd`            | Toma un input y lo guarda en `rd`. |
| `0xE`  | `out rs2`          | Produce un output de valor `rs2`.  |
| `0xF`  | `halt`             | Detiene la ejecución.              |

## Entorno de simulación

El ISA se simula bajo una memoria plana de 64K words. El programa se carga a partir del address 0.

## Uso

```bash
cmake -S . -B build
cmake --build build

build/toy-simulator <archivo>
```

El archivo binario debe ser un volcado raw de words de 16 bits (endianness nativa del host).
