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
For negative immediates use a preceding minus sign (`-`).

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

The initializer list has to be non-empty and can have elements of those types:

- A string literal denoted by double quotes, e.g. `"foo"`. The string is not null terminated.

- An immediate.

A variable can refer to a sequence of words with a comma separated list,
e.g. `str.B = "foo", 0x0A, "bar", 0x00` to include a new line character between
`"foo"` and `"bar"` and end the string with a null terminator.

Note that all elements of an initializer are stored in a whole word per default
which also applies to strings. To use the less significant bytes use a width specifier.
The width specifier applies to all elements of the initializer list.
This means that `str = "foo", 0x00` will not store the bytes `0x66, 0x6f, 0x6f, 0x00` but rather
`0x66, 0x00, 0x00, 0x00, 0x6f, 0x00, ...`.

To use the variable somewhere in the code segment precede its name with a dollar sign (`$`).

Example:
```
segment: data
  bytes.B = 0xEF, 0xBE, 0xAD, 0xDE
  same_word = 0xDEADBEEF

  hello_world.B = "Hello, World!", 0x00
```
