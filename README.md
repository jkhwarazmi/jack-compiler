# JACK Compiler

A compiler for the [Jack](https://www.nand2tetris.org/course) language. It compiles a directory of `.jack` files into VM code that runs on the nand2tetris VM emulator.

## Features

- Lexer with error reporting for unterminated comments and strings, new lines in strings and illegal symbols
- Recursive descent parser covering the full Jack grammar
- Symbol tables with checks for undeclared and redeclared identifiers, including references across files and calls to the standard library
- VM code generation for all Jack statements and expressions, strings, arrays, constructors, methods and functions

## Building

```sh
make
```

## Usage

```sh
./jackc <directory>
```

Each `Foo.jack` in the directory is compiled to `Foo.vm` alongside it. For example:

```sh
$ ./jackc tests/codegen/Seven
$ cat tests/codegen/Seven/Main.vm
function Main.main 0
push constant 1
push constant 2
push constant 3
call Math.multiply 2
add
call Output.printInt 1
pop temp 0
push constant 0
return
```

If there is an error, the compiler prints it and exits with status `1`:

```sh
$ ./jackc tests/symbols/UNDECLAR_VAR
Line 8 at or near t: undeclared identifier
```

## Project structure

```
src/
  lexer.c / lexer.h
  parser.c / parser.h
  symbols.c / symbols.h
  compiler.c / compiler.h
tests/
  lexer/
  parser/
  symbols/
  codegen/
```

## Testing

```sh
make test           # runs all of the below
make test-lexer
make test-symbols
make test-codegen
make clean
```

The parser grader in `tests/parser` only works with the parser on its own, so it isn't included in `make test`.

## Limitations

- Fixed table sizes: 100 classes, 100 symbols per class and per subroutine, and 2048 lines of VM output
- Compilation stops at the first error
- Standard library signatures are hard-coded in `symbols.h`
