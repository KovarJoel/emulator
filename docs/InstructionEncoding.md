# Instruction Encoding

- 8 bits for OP code (up to 128 instructions)
- 4 bits for register address (16 registers)
- 20 bits for lables/jump targets, they get sign extended
- 16 bits for immediates, they get sign extended if not explicitly mentioned otherwise

## General Encoding
Used for e.g. arithmetic, logic and load/store instructions.

### `R`-Type
With two register sources:
```c
    [   OP   ][ DEST ][ SRC1 ][ SRC2 ][  PADDING  ]
//    8 bits   4 bits  4 bits  4 bits    12 bits
```

### `I`-Type
With a register and an immediate source:
```c
    [   OP   ][ DEST ][ SRC1 ][   IMMEDIATE   ]
//    8 bits   4 bits  4 bits      16 bits
```

**Note:** For the store instruction, the `DEST` actually refers to the source
register and `SRC1` and `SRC2`/`IMMEDIATE` refer to the memory location being written to.

## Branch Encoding
### `B`-Type
Used for conditional branches with immediate targets.
```c
    [   OP   ][ CONDITION ][    JUMP TARGET     ]
//    8 bits     4 bits          20 bits
```

### `P`-Type
Used for jumps with register targets.
```c
    [   OP   ][ LINK ][ TARGET ][     OFFSET     ]
//    8 bits   4 bits   4 bits        16 bits
```

### `J`-Type
Used for jumps with immediate targets.
```c
    [   OP   ][ LINK ][      JUMP TARGET       ]
//    8 bits   4 bits          20 bits
```
