# Assembly
Keywords, Mnemonics, width specifiers, hexadecimal digits and register names
are case insensitive.

Whitespace is ignored with the following two exceptions: 
1. there has to be whitespace between the instruction mnemonic and the first argument
2. there has to be a whitespace between the `function` keyword and the functions name
3. each segment specifier, label, instruction and data assignment has to be on a single line.

## Comments
The assembler supports comments which start with a hash (`#`).
Everything in a line after the hash is interpreted as comment.

## Immediates
Immediates are assumed to be in decimal representation.
For hexadecimal prefix with `0x...`, for octal prefix with `0o...`,
for binary prefix with `0b...`.
Immediates are always interpreted as unsigned and therefore zero extended.
To use negative values for immediates,
load them into a register as follows: `SUB dest, r0, imm`.

## Segments
There is a code and a data segment which may be interleaved in the assembly code.
A segment is denoted with the `segment` keyword followed a colon and either
code or data.

Label and variable names can contain any of the following characters: `a-z`, `A-Z`, `0-9` and
underscores (`_`) and may not start with a digit or underscore.

### Code
The code segment can contain instructions and labels. Labels are further divided into
function labels and branch labels.

Function labels are created with a preceding `function` keyword and a colon afterwards.
Functions can not be nested. A functions end is marked by either another function or segment
or the end of the file.

All labels without the `function` keyword are function-local branch labels.

Function lables have to be unique for the whole program, branch labels may be repeated
in different functions but can not be used if a function or local branch with the same
label already exists.

Example program:
```
segment: code

; prints the digits from 0-9 to the console
function main:
    MOV   r1, 48
    MOV   r2, 0xFF
    MOV   r3, 0

  loop:
    ST    r2, 0x1000(r3)
    ST    r2, 0x1001(r3)
    ST    r2, 0x1002(r3)
    ST    r1, 0x1003(r3)
    
    ADD   r3, r3, 4
    INC   r1

    CMP   r1, 57
    BLT   loop

    HALT
```

### Data
Global variables can be created in the data segment and they are in the form
```
variable_name = <initializer>
```
The `variable_name` stores the address of the first byte of the data.

The initializer can be one of those types.

- A string denoted by double quotes, e.g. `str = "foo"`. The string is not null terminated.
Additional bytes can be used with a comma separated list, e.g. `str = "foo", 0x0A, "bar", 0x00`
to include a new line character between `"foo"` and `"bar"` and end the string with a null
terminator.

- An integer initialized with a constant in hexadecimal, decimal, octal or binary format.
For decimal values use a minus (`-`) to indicate negative values and sign extension.
Decimal values without a minus or values in other bases are zero extended.
The initializer can contain multiple values in a comma separated list.

Strings use a single byte for each character per default, integers use a full word per default.
To change the width, the name may be suffixed with a width specifier similar to the
instructions. This specifier will apply to all elements in the initializer.

Example:
```
segment: data
bytes.B = 0xEF, 0xBE, 0xAD, 0xDE
same_word = 0xDEADBEEF
```
