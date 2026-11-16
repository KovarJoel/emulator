# Instruction Encoding

- 7 bits for OP code (up to 128 instructions)
- 1 bit for addressing mode (immediate or register)
- 4 bits for register address (16 registers)
- 20 bits for lables/jump targets, they get sign extended
- 16 bits for immediates, they get sign extended if not explicitly mentioned otherwise

## General Encoding
Used for e.g. arithmetic, logic and load/store instructions.

With two register sources:
```c
    [   OP   ][ MODE ][ DEST ][ SRC1 ][ SRC2 ][  PADDING  ]
//    7 bits   1 bit   4 bits  4 bits  4 bits    12 bits
```

With a register and an immediate source:
```c
    [   OP   ][ MODE ][ DEST ][ SRC1 ][   IMMEDIATE   ]
//    7 bits   1 bit   4 bits  4 bits      16 bits
```

**Note:** For the store instruction, the `DEST` actually refers to the source
register and `SRC1` and `SRC2`/`IMMEDIATE` refer to the memory location being written to.

## Branch Encoding
Used for conditional branches with immediate targets.
```c
    [   OP   ][ PADDING ][ CONDITION ][    JUMP TARGET     ]
//    7 bits     1 bit      4 bits           20 bits
```

Used for jumps with immediate targets.
```c
    [   OP   ][ MODE ][ LINK ][      JUMP TARGET       ]
//    7 bits   1 bit   4 bits          20 bits
```

Used for jumps with register targets.
```c
    [   OP   ][ MODE ][ LINK ][ TARGET ][ OFFSET ]
//    7 bits   1 bit   4 bits   4 bits   16 bits
```
