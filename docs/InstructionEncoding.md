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
    [   OP   ][ REG1 ][ REG2 ][ REG3 ][   FUNCT   ]
//    8 bits   4 bits  4 bits  4 bits    12 bits
```

### `I`-Type
With a register and an immediate source:
```c
    [   OP   ][ REG1 ][ REG2 ][   IMMEDIATE   ]
//    8 bits   4 bits  4 bits      16 bits
```

## Branch Encoding
### `B`-Type
Used for conditional branches with immediate targets.
```c
    [   OP   ][ CONDITION ][    JUMP TARGET     ]
//    8 bits     4 bits          20 bits
```

### `J`-Type
Used for jumps with immediate targets.
```c
    [   OP   ][ REG1 ][      JUMP TARGET       ]
//    8 bits   4 bits          20 bits
```
