# ctprintf

`ctprintf` is a small C++20, header-only formatter for embedded and
freestanding-oriented applications. It provides familiar `printf`-style
formatting, validates literal format strings and argument types at compile
time, and writes one character at a time to an application-provided output.

The library is intended for diagnostic output such as UART, ITM/SWO, log
buffers, or host-side test buffers. It does not use `printf`, iostreams,
`std::string`, or dynamic allocation itself.

## Features

- C++20 header-only library
- Compile-time validation of literal format strings and argument types
- No dynamic allocation in the formatter
- No dependency on libc formatting functions
- Pluggable output through a single `put(char)` operation
- Integer, character, string, pointer, width, flag, and escaped-percent
  formatting

## Quick start

```cpp
#include <ctprintf/format.hpp>

struct Uart {
    void put(char character)
    {
        // Transmit character.
    }
};

int main()
{
    Uart uart;

    ctprintf::format(
        uart,
        "PC=%08x LR=%08x\n",
        0x08001234U,
        0x08005678U);
}
```

The output is:

```text
PC=08001234 LR=08005678
```

## Output interface

The first argument to `ctprintf::format` can be any type that provides:

```cpp
void put(char character);
```

For example, a fixed-size buffer can be used in a test or a logging adapter:

```cpp
struct BufferOutput {
    char *buffer;
    std::size_t position = 0;

    void put(char character)
    {
        buffer[position++] = character;
    }
};
```

`ctprintf` does not own the output or perform bounds checking; the output type
is responsible for transport, storage, synchronization, and capacity handling.

## Format strings

Format strings must be string literals. They are validated during compilation:
the number of conversions must match the number of arguments, and each
argument must have a supported type for its conversion.

```cpp
ctprintf::format(output, "value=%08x\n", 42U); // Valid.
ctprintf::format(output, "value=%08x\n", "42"); // Compile-time error.
```

The following conversions are supported:

| Conversion | Accepted argument | Description |
| --- | --- | --- |
| `%d`, `%i` | Signed integral type | Signed decimal |
| `%u` | Unsigned integral type, excluding `bool` | Unsigned decimal |
| `%o` | Unsigned integral type, excluding `bool` | Octal |
| `%x` | Unsigned integral type, excluding `bool` | Lowercase hexadecimal |
| `%X` | Unsigned integral type, excluding `bool` | Uppercase hexadecimal |
| `%c` | Integral type | Character |
| `%s` | Type convertible to `const char *` | Null-terminated string |
| `%p` | Object pointer, `void` pointer, or `nullptr` | Pointer in hexadecimal |
| `%%` | No argument | Literal percent sign |

`%s` formats a null pointer as `(null)`. `%p` always includes a `0x` prefix;
for example, `nullptr` is formatted as `0x0`.

### Flags and width

The formatter supports the following flags and a decimal minimum field width:

| Option | Meaning |
| --- | --- |
| `-` | Left-align within the field width |
| `+` | Prefix non-negative signed decimal values with `+` |
| space | Prefix non-negative signed decimal values with a space |
| `#` | Add an octal or hexadecimal prefix where applicable |
| `0` | Pad numeric values with zeroes when not left-aligned |
| width | Minimum field width, for example `%08x` or `%-6s` |

Precision, length modifiers, floating-point conversions, positional arguments,
and runtime-provided format strings are not supported.

## Build and test

Requirements:

- A C++20-capable compiler
- CMake 3.25 or newer
- GoogleTest, when building the test suite

Configure and build the library:

```bash
cmake -S ctprintf -B build
cmake --build build
```

To build and run the tests, enable them explicitly:

```bash
cmake -S ctprintf -B build -DENABLE_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Creating a package

CPack creates a Zstandard-compressed tarball containing the installable headers,
CMake package files, README, and license:

```bash
cpack --config build/CPackConfig.cmake
```

The archive is written to the build directory and is named
`ctprintf-<version>-<system>.tar.zst`, for example
`ctprintf-0.1.0-Linux.tar.zst`.

## Installation and CMake integration

Install the header and CMake package files with:

```bash
cmake --install build --prefix /desired/prefix
```

An application can then consume the installed package:

```cmake
find_package(ctprintf CONFIG REQUIRED)

target_link_libraries(my_application PRIVATE ctprintf::ctprintf)
```

The exported target supplies the include directory and requires C++20.


## License

MIT License. See [LICENSE](LICENSE).
