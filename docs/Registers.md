# Registers
The architecture supports 16 registers, 10 of which are general purpose.

- R0 constant 0 register
- R1 - R10 general purpose
- R11, R12 reserved
- R13 stack pointer
- R14 flags register
- R15 program counter

The flags register and the program counter must not be directly accessed.

## Register layout
Each register is word sized (32 bits). When using instructions that operate on halfwords or bytes the less significant 16 or 8 bits respectively are used.

## Extending
All signed writes to partial registers are sign extended.
All unsigned writes to partial registers are zero extended.

## Flags register bits (from LSB to MSB)
- zero
- negative
- overflow
- carry
- reserved ...
