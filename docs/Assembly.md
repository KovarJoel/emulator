# Assembly
**Make sure to take a look at the [examples](../examples/) directory.**

Keywords, Mnemonics, width specifiers, hexadecimal digits and register names
are case insensitive.

## Comments
The assembler supports comments which start with a hash (`#`).
Everything in a line after the hash is interpreted as comment.
Note that a hash inside of a string literal is not considered a comment
but part of the string.

## Immediates
Immediates are assumed to be in decimal representation.
For hexadecimal prefix with `0x...`, for octal prefix with `0o...`,
for binary prefix with `0b...`.
They are stored using 24 bits and always sign extended, except for jumps (`JMP`) and branches (`Bxx`)
where they are zero extended.

For negative immediates use a preceding minus sign (`-`).

For unsigned immediates add the character `u` after the immediate value. Positive values do
not need to be unsigned in general but only if the positive value is not representable in a
signed immediate. Note that unsigned immediates can be silently sign extended if their most
significant bit is set, even though they are unsigned.

Alternatively strings with a length of one can be used to have the immediates value
be set to the characters unsigned ascii value. The character will be placed in the least
significant byte and the upper bytes are set to zero. Strings are denoted with
double quotes at the front and back, e.g. `"A"`.

## Segments
There is a code and a data segment which may be interleaved in the assembly code.
A segment is denoted with the `segment` keyword followed by a colon and either
code or data.

Label and variable names can contain any of the following characters: `a-z`, `A-Z`, `0-9` and
underscores (`_`) and may not start with a digit or underscore.

### Code
The code segment may contain instructions and labels. Labels are further divided into
function labels and branch labels.

Function labels are created with a preceding `function` keyword and a colon afterwards.
Functions can not be nested. A functions end is marked by either another function or segment
or the end of the file.

All labels without the `function` keyword, so just a label name followed by a colon,
are function-local branch labels.

Function lables have to be unique for the whole program, branch labels may be repeated
in different functions but can not be used if a function or local branch with the same
label already exists.

### Data
Global variables can be created in the data segment and they are in the form
```
variable_name = <initializer-list>
```
The `variable_name` stores the address of the first byte of the data.

The initializer list has to be non-empty and consits of a comma separated list
of immediates, with the difference that strings can contain multiple characters.
If a string with multiple characters is used, each character is interpreted as its
own immediate value. Note that strings are not null terminated.

The variable definition can include an optional width specifier, similar to the load and store
instructions. The width applies to all values in the initializer list.
All elements are stored in a whole word per default.

Examle: `str.B = "foo", 0x0A, "bar", 0x00` to include a new line character between
`"foo"` and `"bar"` and end the string with a null terminator.

Note that the default width also applies to strings. To use the less significant bytes use a width specifier.
This means that `str = "foo", 0x00` will not store the bytes `0x66, 0x6f, 0x6f, 0x00` but rather
`0x66, 0x00, 0x00, 0x00, 0x6f, 0x00, ...`.

To use the variable somewhere in the code segment precede its name with a dollar sign (`$`)
to get the address of its first byte as an immediate value.

Example:
```
segment: data
  bytes.B = 0xEF, 0xBE, 0xAD, 0xDE
  same_word = 0xDEADBEEF

  hello_world.B = "Hello, World!", 0x00
```
