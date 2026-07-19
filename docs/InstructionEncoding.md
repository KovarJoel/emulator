# Instruction Encoding

- 8 bits for OP code (up to 256 instructions)
- 4 bits for register address (16 registers)
- 1 bit for addressing mode (immediate or register)
- 24 bits for lables/jump targets
- 24 bits for immediates
- 2 bits for operand width (byte, half word, word, reserved)

## General Encoding
Used for e.g. arithmetic, logic and load/store instructions.
```c
    [   OP   ][ WIDTH ][ DEST ][ SRC1 ][ MODE1 ][ SRC2 ][ MODE2]
//    8 bits   2 bits   4 bits  4 bits   1 bit   4 bits   1 bit
//    total = 24 bits
    [                   IMMEDIATE1                     ]  // optional
    [                   IMMEDIATE2                     ]  // optional
//                       24 bits
```

**Note:** For the store instruction, the `DEST` actually refers to the source
register and `SRC1` and `SRC2` refer to the memory location being written to.

## Branch Encoding
Use for conditional branches with immediate targets.
```c
    [   OP   ][              JUMP TARGET               ]
//    8 bits                   24 bits
//    total = 32 bits
```
