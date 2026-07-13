# Memory model
- byte addressable
- little endian
- unaligned access ok
- 2^24 bytes = 16 MiB RAM
- pages of 4096 = 0x1000 bytes

## NULL page and invalid pages
Addresses `0` to `0x0fff` are invalid (NULL page). Accessing an invalid page will
result in immideate termination of the running process.

## Memory Mappings
Addresses from `0x1000` to `0xffff` are reserved for memory mappings.

### Console
The console mapping ranges from `0x1000` to `0x6fff`. The cells range from `0x1000` to `0x63ff`.
After that there is additional data relevant for the console. The additional data is in
the following format:
```c
struct {
  int32_t next_x;
  int32_t next_y;
  // reserved ...
};
```
Where `(next_x, next_y)` represents the position where the next write will happen,
typically one to the right of the last written to cell, or the beginning of the next row
if the previous one was filled.

The console mapping supports 128 * 42 cells.
Each cell corresponds to 4 bytes representing the texts foreground color and the symbol.
The color is in RGB format with 1 byte per channel (so a total of 2^24, ca. 16 * 10^6, colors are possible). The symbols are represented as ASCII (maybe CP437 later on).

The memory layout of each cell is equivalent to the following:
```c
struct Cell {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t symbol;
};
```

The indexing starts from the top left at `(0,0)` and the bottom right is at `(127, 41)`.
Accessing a cell at position `(x,y)` is done like `cells[y * 128 + x]`.

### Keyboard and mouse input
The input mapping ranges from `0x7000` to `0x7fff`.

## Code and static data
The binary data of the program must follow this structure, containing these four sections with increasing starting addresses for each. Each section shall be page aligned.

1. Header section
2. Instruction section
3. Data section
4. Ram section

The first page is reserved only for the header section and starts at address `0x10000`.
The header section contains a header with general information about the loaded program and its structure. The header itself is structured in the following way and is read only:

```c
struct Header {
  const unsigned char tag[8] = { 'e', 'm', 'u', 'l', 'a', 't', 'o', 'r' };
  const uint32_t version;
  const uint32_t code_begin = 0x11000;
  const uint32_t data_begin;
  const uint32_t ram_begin;
  const uint32_t entry_point;
  // reserved ...
};
```

Starting from the second page, the binary shall contain executable and readable but not writeable instruction memory.

Everything after the instruction section shall be readable and writeable but not executable.
Static data is placed directly after the instruction memory section.
After the last page for the data section everything else is freely accessible RAM including the stack. The RAM is not guaranteed to be initialized to a specific value.

## Stack
The initial stack pointer has a value of `0x1000000` (end of the RAM).
The stack grows downwards. There is no strict limit on the size of the stack.
The stack can grow as long as it does not exceed `ram_begin`. Therefore a smaller program
can make use of a bigger stack.
