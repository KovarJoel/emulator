# Registers
The architecture supports 16 registers, 10 of which are general purpose.

- R0 constant 0 register
- R1 - R10 general purpose
- R11 reserved
- R12 `RA` return address
- R13 `SP` stack pointer
- R14 `FLAGS` flags register
- R15 `PC` program counter

The flags register and the program counter shall not be directly accessed.

## Register layout
Each register is word sized (32 bits). When using instructions that operate on halfwords or bytes the less significant 16 or 8 bits respectively are used.

## Flags register bits (from LSB to MSB)
- zero
- sign
- carry
- overflow
- reserved ...
