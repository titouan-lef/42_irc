# Tester

## Description

Before starting the project, we created this tester that allowed us to perform unit tests. This allowed us to verify that the code was working properly after each update and to test new features. So, the tests were added gradually, throughout the project. These unit tests are used to test our internal functions and the final program.

This tester uses Valgrind to detect memory leaks.

## Instructions

All commands are executed in the unit_tests directory.

> [!TIP]
> If valgrind aren't installed, you can use the [nix terminal](../README.md#nix)

### Makefile

Use the provided `Makefile` to compile, run and manage the tester:

| Command | Description |
|--------|-------------|
| `make` / `make all` | Compile and run the tester |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and executables |
| `make re` | Clean, recompile and run the tester |
