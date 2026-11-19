
# Simulador RISC-V RV32I

Simulador del conjunto de instrucciones RISC-V de 32 bits (RV32I), desarrollado en C++.

## Descripción

El simulador permite cargar programas compilados en formato binario, ejecutar instrucciones RISC-V y consultar el estado del procesador.

### Instrucciones implementadas

| Tipo | Instrucciones |
|---|---|
| Aritméticas y lógicas | `add`, `sub`, `and`, `or`, `xor`, `sll`, `srl`, `sra`, `slt`, `sltu` |
| Inmediatas | `addi`, `andi`, `ori`, `xori`, `slli`, `srli`, `srai`, `slti`, `sltiu` |
| Carga | `lb`, `lh`, `lw`, `lbu`, `lhu` |
| Almacenamiento | `sb`, `sh`, `sw` |
| Saltos condicionales | `beq`, `bne`, `blt`, `bge`, `bltu`, `bgeu` |
| Saltos | `jal`, `jalr` |
| Tipo U | `lui`, `auipc` |


## Uso

Para ejecutar el simulador:

```bash
./sim programa.bin
```

### Comandos disponibles

| Comando | Descripción |
|---|---|
| `pc` | Muestra el contador de programa |
| `regs` | Muestra todos los registros |
| `regs x1 x2` | Muestra registros específicos |
| `step` | Ejecuta una instrucción |
| `run N` | Ejecuta hasta N instrucciones |
| `mem DIRECCION N` | Muestra N bytes de memoria |
| `reset` | Reinicia la CPU y recarga el programa |
| `help` | Muestra los comandos disponibles |
| `exit` | Finaliza el simulador |

## Pruebas realizadas

Se verificó el funcionamiento del simulador utilizando 4 programas:

| Programa | Resultado esperado |
|---|---|
| `riscvtest.bin` | Almacena el valor 25 en la dirección `0x64` |
| `quicksort.bin` | Ordena el arreglo `{6, 4, 3, 2, 1, 8, 9}` |
| `symmetric.bin` | Devuelve `x10 = 1`, indicando que el árbol es simétrico |
| `fibonacci.bin` | Calcula los primeros diez términos de Fibonacci, obteniendo `x2 = 34` |

### Ejemplo de ejecución

```bash
./sim riscvtest.bin
```

Dentro del simulador:

```text
run 100
mem 0x64 4
pc
exit
```

Resultado:

```text
0x00000064: 19 00 00 00
PC = 0x00000050
```