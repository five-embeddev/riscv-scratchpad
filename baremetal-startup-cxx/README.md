Example device independent startup file written in C++ for RISC-V.

Based on a few sources, a little inspired by [Christopher Kormanyos's
Real Time C++](https://github.com/ckormanyos/real-time-cpp).

The objective is to enter a main() function and enable a simple periodic ISR 
handler, while staying in machine mode.

Source Files:

- src/startup.cpp          : Entry point from reset. Set up C++ runtime environment.
- src/main.cpp             : Example main program. Configures timer interrupt for 1s periodic interrupt.
- src/timer.hpp            : Device independent C++ driver for the RISC-V machine mode timer.
- src/riscv-csr.hpp        : C++ class abstraction to access RISC-V CSRs (Generated file)
- src/riscv-interrupts.hpp : List of RISC-V machine mode interrupts.

Build Files:

- src/CMakeLists.txt       : CMake build file.
- Makefile                 : Makefile to configure and run CMake, and upload and debug on SiFive HiFive revb board.

Other Files:

- src/linker.lds           : Linker script for SiFive HiFive revb board (from the metal environment).
