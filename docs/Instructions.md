# Instructions

The following addressing modes are supported for source operands:
- register
- immediate up to 16 bits

The `FLAGS` register may observe modifications by any instruction.
Conditional branch instructions must always be placed immediately after a comparison,
or after another instruction modifying the `FLAGS` register.
Using the `FLAGS` register as destination operand for an instruction is UB.

## Memory
The following addressing modes are supported for both load and store memory accesses
for the operand refering to the memory location:
- Indirect `(r1)`, equivalent to `Mem[r1]`, same as `(r1)[r0]`.
This uses the `R`-Type instructions.
- Indirect with immediate displacement `100(r1)`, equivalent to `Mem[r1 + 100]` (use R0 for fixed address).
This uses the `I`-Type instructions.
- Indexed `(r1)[r2]`, equivalent to `Mem[r1 + d * r2]`, where `d` is the width of the instruction.
This uses the `R`-Type instructions.

### Load
```
LD dest, src
LDH dest, src
LDHZX dest, src
LDB dest, src
LDBZX dest, src

LDI dest, src
LDHI dest, src
LDHZXI dest, src
LDBI dest, src
LDBZXI dest, src
```
Loads a value stored at `src` into `dest`.
The source must refer to an address where at least `d` bytes are available for reading.

`LD` sign-extends with smaller than word size operands.
`LDZX` zero-extends with smaller than word size operands.

When only reading a partial word, the following instructions can be used:
`LDB`/`LDBZX` for byte, `LDH`/`LDHZX` for half word e.g. `LDH` for reading a signed half word (16 bits with sign extension).

Reading from an address range without read permissions leads to immediate termination of the program.

**Zero Flag:** Set iff the destination is 0.

**Sign Flag:** Set iff the MSB of the destination is 1.

### Store
```
ST dest, src
STH dest, src
STB dest, src

STI dest, src
STHI dest, src
STBI dest, src
```
Stores a value in `src` into `dest`.
The destination must refer to an address where at least `d` bytes can be written to.
The source must refer to a register. Storing immediates is not possible.

Storing a less than word sized data type results in only the less significant bytes
being written to memory.

When only writing a partial word, the following instructions can be used:
`STB` for byte, `STH` for half word e.g. `STH` for writing a half word.

Writing to an address range without write permissions leads to immediate termination of the program.

## Arithmetic

Instructions ending in `I` use the `I`-Type encoding with immediate values.
All other arithmetic instructions use the `R`-Type encoding with two register operands.

### Add
```
ADD dest, src1, src2
ADDI dest, src1, src2
```
Stores the result of `src1 + src2` into the destination register.
If the result doesn't fit into a register of the operand size then the MSB of the result
is lost.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Set iff the MSB of the result is 1.

**Carry Flag:** Set iff the unsigned sum requires more bits than the operand size.

**Overflow Flag:** Set iff the MSB of both source operands is the same but the MSB
of the result is different.

### Subtract
```
SUB dest, src1, src2
SUBI dest, src1, src2
```
Stores the result of `src1 - src2` into the destination register.
If the result doesn't fit into a register of the operand size then the MSB of the result
is lost.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Set iff the MSB of the result is 1.

**Carry Flag:** Set iff a borrow is needed for the unsigned subtraktion, i.e.
`src1` is less than `src2`.

**Overflow Flag:** Set iff the MSB of the result is not the same as the MSB of `src1` and
the MSBs of `src1` and `src2` are different, i.e. the result would not be representable
with the operand width.

### Multiply
```
MUL dest, src1, src2
MULI dest, src1, src2
```
Stores the result (only the lower word) of `src1 * src2` into the destination register.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Set iff the MSB of the result is 1.

**Carry Flag:** Set iff the result would exceed the operand width.

**Overflow Flag:** Set iff the MSB of the result is not the same as the exclusive-or combination
of the MSBs of `src1` and `src2`. 

### Divide
```
DIV   dest, src1, src2
DIVU  dest, src1, src2
DIVI   dest, src1, src2
DIVUI  dest, src1, src2
```
Stores the result of `src1 / src2` into the destination register.
If `src2` is `0`, the program is terminated immediately.
`DIV` calculates the signed division, `DIVU` the unsigned division.
Results are truncated towards zero.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Only set by the signed version. Set iff the MSB of the result is 1,
i.e. both source operands had different signs.

### Remainder
```
REM   dest, src1, src2
REMU  dest, src1, src2
REMI   dest, src1, src2
REMUI  dest, src1, src2
```
Stores the remainder of `src1 / src2` into the destination register.
If `src2` is `0`, the program is terminated immediately.
`REM` calculates the remainder from the signed division, `REMU` calculates the reaminder
from the unsigned division.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Only set by the signed version. Set iff the MSB of the result is 1,
i.e. `src1` is negative. 

## Shifts and Rotations
The number of positions to shift/rotate is always interpreted as unsigned value.

### Rotate Left
```
ROL dest, src1, src2
ROLI dest, src1, src2
```
Rotates the value in `src1` to the left by `src2` positions and stores the result in the
destination operand.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Set iff the MSB of the result is 1.

### Rotate Right
```
ROR dest, src1, src2
RORI dest, src1, src2
```
Rotates the value in `src1` to the right by `src2` positions and stores the result in the
destination operand.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Set iff the MSB of the result is 1.

### Logical Left shift
```
SLL dest, src1, src2
SLLI dest, src1, src2
```
Shifts the value in `src1` to the left (towards the MSB) by `src2` positions and stores
the result in the destination operand.
For each shifted position, the MSB is shifted into the carry flag and the LSB is cleared.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Set iff the MSB of the result is 1.

**Carry Flag:** Set to the last bit shifted out of the destination register.

**Overflow Flag:** Set iff the MSB of the result and the MSB of `src1` are different.

### Logical Right shift
```
SRL dest, src1, src2
SRLI dest, src1, src2
```
Shifts the value in `src1` to the right (towards the  LSB) by `src2` positions and stores
the result in the destination operand.
For each shifted position, the LSB is shifted into the carry flag and the MSB is cleared.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Set iff the MSB of the result is 1.

**Carry Flag:** Set to the last bit shifted out out of the destination register.

**Overflow Flag:** Set iff the MSB of the result and the MSB of `src1` are different.

### Arithmetic Right shift
```
SRA dest, src1, src2
SRAI dest, src1, src2
```
Shifts the value in `src1` to the right (towards the LSB) by `src2` positions and stores
the result in the destination operand.
For each shifted position, the LSB is shifted into the carry flag and the MSB is set to
the MSB of `src1`, acting as a sign extension.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Set iff the MSB of the result is 1.

**Carry Flag:** Set to the last bit shifted out of the destination register.

## Bitwise Logical Operations
### And
```
AND dest, src1, src2
ANDI dest, src1, src2
```
Computes the bitwise `and` of `src1` and `src2` and stores the result in the destination register.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Set iff the MSB of the result is 1.

### Or
```
OR dest, src1, src2
ORI dest, src1, src2
```
Computes the bitwise `or` of `src1` and `src2` and stores the result in the destination register.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Set iff the MSB of the result is 1.

### Exclusive Or
```
XOR dest, src1, src2
XORI dest, src1, src2
```
Computes the bitwise `xor` of `src1` and `src2` and stores the result in the destination register.

**Zero Flag:** Set iff the result is 0.

**Sign Flag:** Set iff the MSB of the result is 1.

## Branches
If a branch target is in a memory segment other than the code segment the program will be
terminated immediately.

Labels are always relative to the current program counter if not explicitly mentioned otherwise.

### Jump
```
JMP link, target
JMPR link, target, offset
```
Unconditionally jumps to the specified target.
The address of the next instruction is placed into the link register.

`JMP` jumps to the PC-relative immediate target address. This uses `J`-Type instruction encoding.
`JMPR` jumps to the absolute address stored in the target register and adds the offset to it.
This uses `P`-Type instruction encoding.

### Branch
```
Bxx label
```
Checks the condition and jumps to the specified immediate label if the condition is met.
Branches can not be used with registers as source for the label.
This uses the `B`-Type instruction encoding.

| Instruction | Description |
| ----------- | ----------- |
| **equal:**
| BE          | Branch if equal (ZF=1)
| BEZ         | Branch if equal zero (ZF=1), pseudoinstruction
| BNE         | Branch if not equal (ZF=0)
| BNEZ        | Branch if not eual zero (ZF=0), pseudoinstruction
| **signed:**
| BGT         | Branch if greater than (ZF=0 and SF=OF)
| BLT         | Branch if less than (ZF=0 and SF!=OF)
| BGE         | Branch if greather than or equal (ZF=1 or SF=OF)
| BLE         | Branch if less than or equal (ZF=1 or SF!=OF)
| **unsigned:**
| BGTU        | Branch unsigned if greater than (ZF=0 and CF=0)
| BLTU        | Branch unsigned if less than (CF=1)
| BGEU        | Branch unsigned if greather than or equal (CF=0)
| BLEU        | Branch unsigned if less than or equal (ZF=1 or CF=1)
| **other:**
| BC          | Branch if carry (CF=1)
| BNC         | Branch if not carry (CF=0)
| BO          | Branch if overflow (OF=1)
| BNO         | Branch if not overflow (OF=0)

## Other
### Halt
```
HALT
```
Terminates the program and returns control to the emulator.
The program can be resumed afterwards as if the `HALT` never happened,
i.e. the whole processors state is guaranteed to not have changed, except the
cycle count and the program counter which were incremented.

### Cycle Count Since Startup
```
CYCL dest
```
Stores the number of completed cycles (excluding the current `CYCL` instruction) in the
destination register. Wraps around to `0` after `UINT32_MAX` cycles.

**Zero Flag:** Set iff the result is 0.

### Random
```
RAND dest, src1, src2
```
Generates a pseudo-random word and stores it in the destination register.
The generated word will be in the unsigned range `[src1, src2]`.

**Zero Flag:** Set iff the result is 0.
**Sign Flag:** Set iff the MSB of the result is 1.

## Pseudo Instructions
(Not implemented yet)

### No-Operation
```
NOP
```
Does nothing, except consuming once cycle. Equivalent to `ADD r0, r0, r0`.

### Move
```
MOV dest, src
```
Stores the source value into the destination value. Equivalent to `ADD dest, r0, src`.

### Call
```
CALL function
```
Equivalent to `JMP ra, function`.

### Return
```
RET
```
Equivalent to `JMP r0, ra`.

### Increment
```
INC reg
```
Increments the value in `reg` which is both a source and destination operand.
Equivalent to `ADD reg, reg, 1`.

### Decrement
```
DEC reg
```
Decrements the value in `reg` which is both a source and destination operand.
Equivalent to `SUB reg, reg, 1`.

### Push
```
PUSH src
```
Pushes the source value onto the stack and updates the stack pointer accordingly. If this
operation would cause the stack to overflow, i.e. overwrite something in the static data region,
the program will terminate immediately.

Equivalent to `SUB sp, sp, d; ST (sp), src` where `d` is the operand width.

Use `PUSH.x` where `x` is `B`, `H` or `W` for specifying other operand sizes.

### Pop
```
POP dest
```
Pops the top of the stack into the destination register and updates the stack pointer accordingly.
If this operation would cause the stack to underflow, i.e. popping a value outside of the RAM
boundarys, the program will terminate immediately.

Equivalent to `LD dest, (sp); ADD sp, sp, d` where `d` is the operand width.

Use `POP.x` where `x` is `B`, `H` or `W` for specifying other operand sizes.

### Compare
```
CMP src1, src2
```
Compares the two source values and sets/clears the according flags in the `FLAGS` register.
Equivalent to `SUB r0, src1, src2`.

### Arithmetic Left shift
```
SLA dest, src1, src2
```
Equivalent to `SLL dest, src1, src2`.
