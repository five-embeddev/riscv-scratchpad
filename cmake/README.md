Example [CMake](https://cmake.org/) build script for
the [RISC-V](https://riscv.org/) targeting embedded bare metal applications.

A small makefile will build the CMake build scripts in the build directory and run them.

Input files:

- Makefile           : Invoke CMake and the generated makefile. 
- cmake/riscv.cmake  : Locate a cross compiler and set the RISC-V variant.
- src/CMakeLists.txt : Build a simple test application with custom linker file and output of map, hex and disassembly files.
- src/test.c         : Example application file with main() function.
- src/startup.c      : Dummy bare-metal entry and startup. NOT FUNCTIONAL.
- src/dummy.c        : Dummy additional source file.
- include/dummy.h    : Dummy include files.

BSP Files:

- src/test.lds       : Linker script from SiFive

Output:

- build/test.elf     : Target executable
- build/test.hex     : Target hex-file
- build/test.map     : Target linker map file
- build/test.disasm  : Disassembled executable
- build/test.s       : Disassembled test.o object
- build/startup.s    : Disassembled startup.o object

Credits:

Baremetal CMake script for ARM: https://www.valvers.com/open-software/raspberry-pi/bare-metal-programming-in-c-part-3/.
