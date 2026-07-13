# Instructions

All instructions which refer to at least one register can be used with all available integral types.
If no type is specified, `int32_t` is assumed. One can specify a type by appending a suffix to
the instruction mnemonic. This type will be applied to all registers in the instruction where it is used, except specified otherwise.
For unsigned data types use `u`, for signed `s` (default). For full words use `w` (default),
for half words use `h`, for bytes use `b`. Ordering of the suffix is ignored, at most one of
the signednesses and at most one of the widths may be used.
Example: `ADD.uw R1, R2, R3`.

The following addressing modes are supported for memory accesses:
- indirect `ld r1, (r2)`, equivalent to `r1 = Mem[r2]`
- indirect with displacement `ld r1, 100(r2)`, equivalent to `r1 = Mem[r2 + 100]` (use R0 for fixed address)
- indexed `ld r1, (r2)[r3]`, equivalent to `r1 = Mem[r2 + d * r3]` 
- scaled `ld r1, 100(r2)[r3]`, equivalent to `r1 = Mem[100 + r2 + d * r3]`

The following addressing modes are supported for source operands:
- register
- immediate up to 24 bits


Immediates are assumed to be in decimal representation. For hexadecimal prefix with `0x...`, for
octal prefix with `0o...`, for binary prefix with `0b...`. Decimal immediates are negative if they
are prefixed with a minus `-` sign. Immediates in other bases implicitly get the signedness by
observing the 24th bit (MSB).

Mnemonics, type specifiers, hexadecimal digits and register names are case insensitive. 

## Memory
### Load
```
LD dest, src
```
Loads a value stored at `src` into `dest`. The destination must name a general purpose register.
The source must refer to an address where at least `d` bytes, where `d` is the size of the destination operand, are available for reading.

Signedness and width specifiers only apply to the destination operand. All other operands are
interpreted as signed words.

Reading from an address range without read permissions leads to immideate termination of the program.

### Store
```
ST dest, src
```
Stores a value in `src` into `dest`. The source must name a general purpose register. The
destination must refer to an address where at least `d` bytes can be written to.

Signedness and width specifiers only apply to the source operand. All other operands are
interpreted as signed words. There is no zero- or sign-extending for bytes and half words.

Writing to an address range without write permissions leads to immideate termination of the program.


### Push
```
PUSH src
```
Pushes the source value onto the stack and updates the stack pointer accordingly. If this
operation would cause the stack to overflow, i.e. overwrite something in the static data region,
the program will terminate immedeately.

### Pop
```
POP dest
```
Pops the top of the stack into the destination register and updates the stack pointer accordingly.
If this operation would cause the stack to underflow, i.e. popping a value outside of the RAM
boundarys, the program will terminate immedeately.

### Add
```
ADD dest, src1, src2
```
Stores the result of `src1 + src2` into the destination register.

### Subtract
```
SUB dest, src1, src2
```
Stores the result of `src1 - src2` into the destination register.

### Multiplicate
```
MUL dest, src1, src2
```
Stores the result of `src1 * src2` into the destination register.

### Divide
```
DIV dest, src1, src2
```
Stores the result of `src1 / src2` into the destination register.
If `src2` is `0`, the program is terminated immedeately.

### Modulo
```
MOD dest, src1, src
```
Stores the result of `src1 % src2` into the destination register.
If `src2` is `0`, the program is terminated immedeately.

### Increment
```
INC reg
```
Increments the value in `reg` which is both a source and destination operand.
Pseudoinstruction for `ADD reg, reg, 1`.

### Decrement
```
DEC reg
```
Decrements the value in `reg` which is both a source and destination operand.
Pseudoinstruction for `SUB reg, reg, 1`.

## Shifts and Rotations
### Rotate Left
```
ROL dest, src1, src2
```
Rotates the value in `src1` to the left by `src2` positions and stores the result in the
destination operand.

### Rotate Right
```
ROR dest, src1, src2
```
Rotates the value in `src1` to the right by `src2` positions and stores the result in the
destination operand.

### Logical Left shift
```
SLL dest, src1, src2
```

### Logical Right shift
```
SLR dest, src1, src2
```

### Arithmetic Left shift
```
SAL dest, src1, src2
```

### Arithmetic Right shift
```
SAR dest, src1, src2
```

## Bitwise Logical Operations
### And
```
AND dest, src1, src2
```

### Or
```
OR dest, src1, src2
```

### Exclusive Or
```
XOR dest, src1, src2
```

## Branches
### Jump
```
JMP label
```

### Branch
```
Bxx label
```
- b_xx (where xx corresponds to: eq, gt, lt, geq, leq, ov (overflow), c (carry))

## Functions
### Call
```
CALL function
```

### Return
```
RET
```

## Other
### Halt
```
HALT
```
Terminates the program.

### Move
```
MOV dest, src
```
Stores the source value into the destination value. Equivalent to `ADD dest, r0, src`.

### Compare
```
CMP src1, src2
```

### No-Operation
```
NOP
```
Does nothing, except consuming once cycle.

### Cycle Count Since Startup
```
CYCL dest
```
Stores the number of completed cycles (excluding the current `CYCL` instruction) in the
destination register. Always uses an unsigned word, other specifiers are not supported. Wraps around to `0` after `UINT32_MAX` cycles.