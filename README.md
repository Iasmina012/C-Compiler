# C-Compiler

## Overview

This project is a C compiler that includes lexical analysis, syntax analysis, domain analysis, and a stack-based virtual machine. The compiler takes C code as input and generates executable instructions for the virtual machine.

## Features

- **Lexical Analyzer**: Tokenizes C source code, recognizing identifiers, keywords, operators, and constants.
- **Syntax Analyzer**: Parses the tokenized code to check for syntax errors.
- **Domain Analysis**: Manages symbols, types, and scopes.
- **Virtual Machine**: Executes compiled code using a stack-based approach.

## File Structure

- `lexer.h` / `lexer.c` - Handles lexical analysis.
- `parser.h` / `parser.c` - Implements syntax analysis.
- `ad.h` / `ad.c` - Manages symbol tables and types.
- `vm.h` / `vm.c` - Implements a stack-based virtual machine.
- `utils.h` / `utils.c` - Contains utility functions.
- `main.c` - Entry point of the compiler.

## Compilation and Execution

1. Compile the project using a C11-compatible compiler (e.g., GCC or Clang):
   ```sh
   gcc -o compiler main.c lexer.c parser.c ad.c vm.c utils.c -std=c11
   ```
2. Run the compiler with a test file:
   ```sh
   ./compiler test.c
   ```

## Usage

- The compiler processes a C source file and outputs an executable format for the virtual machine.
- The virtual machine interprets and executes the compiled code.
- Use `puts()`, `putc()`, and `put_i()` for output operations.

## Requirements

- C11 or later compiler.
- Standard C libraries.
