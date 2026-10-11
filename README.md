# RPN Calculator with Bitwise Operations

[![Linux build and tests](https://github.com/Spencer695/RPN-Bitwise-Calculator/actions/workflows/linux-ci.yml/badge.svg)](https://github.com/Spencer695/RPN-Bitwise-Calculator/actions/workflows/linux-ci.yml)

A C++17 coursework project implementing a stack-based, unsigned 16-bit calculator. A CSV runner applies commands and compares the resulting top of stack with expected values. The repository includes portable builds and checked-in regression cases.

## Build and run on Linux

Requirements: a C++17 compiler and CMake 3.16 or newer. On Ubuntu, install them with `sudo apt install build-essential cmake`.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/rpn tests/rpn-cases.csv
```

Run commands from the repository root. You can supply a different CSV path as the first argument. The process returns a nonzero status for an unreadable file, malformed data, an empty suite or a failed comparison.

## Supported commands

| Command | Behavior |
| --- | --- |
| `cmd_enter` | Push a value from 0 to 65535 |
| `cmd_top` / `cmd_pop` / `cmd_clear` | Inspect, remove or clear stack entries |
| `cmd_add` | Add two values; reject a result greater than 65535 |
| `cmd_and` / `cmd_or` | Apply bitwise AND / OR |
| `cmd_left_shift` / `cmd_right_shift` | Shift the lower operand by the top operand, with a valid count of 0 to 15 |

For example, entering `3`, entering `4`, then running `cmd_add` produces `7`. This is a command-driven calculator; it does not parse strings such as `3 4 +`. Subtraction, multiplication, division and XOR are not implemented.

## CSV format and safety

```csv
command,value,expected
cmd_enter,3,3
cmd_enter,4,4
cmd_add,0,7
```

Use `0` in the value column for commands that do not need a new value. An expected result of `-999` means the calculator returns no value, including an empty stack or a rejected operation. The simple CSV format requires exactly three columns and does not support quoted fields or embedded commas.

Insufficient operands return no value. Addition overflow and out-of-range shift counts leave the operands on the stack. Left shifts keep the low 16 bits. CSV parsing validates field count, numeric ranges and complete numeric conversion before indexing or narrowing values.

## Tests and implementation

CTest runs a 47-command regression sequence and six tests that require malformed input, empty input or an incorrect expected result to be rejected. The suite covers stack operations, addition overflow, bitwise operations, zero and maximum valid shift counts, and invalid shift counts. Linux CI builds and runs CTest with address and undefined-behavior sanitizers.

The evaluator uses a `std::vector<uint16_t>` stack and returns an optional result through `std::shared_ptr<uint16_t>`. Its stack is static, so the current implementation is intended for one sequential calculator session and is not thread-safe.

## Project origin and scope

This repository began as coursework. The source filename is retained, and the Git history preserves the original coursework. Portfolio improvements add a portable input path, checked-in fixtures, parsing checks, reliable failure statuses, CMake and Linux CI. No embedded hardware or networking is involved in this project.
