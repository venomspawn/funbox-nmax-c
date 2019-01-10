# Solution to `nmax` test problem implemented in C

The project contains solution to `nmax` test problem from [FunBox's test
case](https://dl.funbox.ru/qt-ruby.pdf). Although original problem required a
solution written on Ruby, this one is implemented in pure C (C99 dialect
actually) for speed comparison and just for fun.

## Limitations

The solution was only tested on x86_64 machine with Arch Linux and gcc 8.
64-bits are not actually required, but `uint64_t` support in `stdint.h` is.
`In`-mode of long number implementation (see `source/num.c`) heavily relies on
little-endianness, but that can be easily patched for big endian platforms.
Also, call to `posix_memalign` in `source/num.c` should be patched for
non-POSIX environments.

## Compiling

The solution can be compiled by calling `make` utility in the root directory of
the project after repository cloning. The `nmax` binary should appear after in
the directory.

## Usage

Compiled `nmax` binary supports one optional argument with amount of keeping
numbers. If the argument is absent or not a strictly positive number, default
`100` value is used. Example of invocation:

```
cat your_40GB_file | ./nmax 10000
```

## Additional comments

*   `Makefile` in the root directory supports `clean` and `test` targets. The
    latter runs some simple tests on fixture files, which can be found in
    `spec/fixtures` directory.

*   The solution was found to be faster than [Ruby
    one](https://github.com/venomspawn/funbox-nmax-ruby) by 20-30 times on the
    same developer machine. Actual speedup heavily depends on type of numbers
    in input file: C-solution grinds faster short numbers.
