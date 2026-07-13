# Instruction Encoding

- 8 bits for OP code (up to 256 instructions)
- 4 bits for register address (16 registers)
- 1 bit for addressing mode (immediate or register)
- 24 bits for lables/jump targets
- 24 bits for immediates

## General Encoding
Used for e.g. arithmetic, logic and load/store instructions.
```c
    [   OP   ][ DEST ][ SRC1 ][ MODE1 ][ SRC2 ][ MODE 2][ PADDING ]
//    8 bits   4 bits  4 bits  1 bit    4 bits  1 bit     2 bits
//    total = 24 bits
    [                   IMMEDIATE1                     ]  // optional
    [                   IMMEDIATE2                     ]  // optional
//                       24 bits
```

## Branch Encoding
Use for conditional branches with immediate targets.
```c
    [   OP   ][              JUMP TARGET               ]
//    8 bits                   24 bits
//    total = 32 bits
```
