Example device independent startup file written in C for RISC-V.

The objective is to enter a main() function and enable a simple ISR handler, while staying in machine mode.

Based on a few sources, a little inspired by [ARM's CMSIS](https://github.com/ARM-software/CMSIS_5)

Example device independent startup file written in C for RISC-V.

The objective is to enter a main() function and enable a simple periodic ISR 
handler, while staying in machine mode.

Source Files:

- src/startup.c          : Entry point from reset. Set up C runtime environment.
- src/main.c             : Example main program. Configures timer interrupt for 1sec periodic interrupt.
- src/timer.h            : Device independent driver for the RISC-V machine mode timer.
- src/timer.c            : Device independent driver for the RISC-V machine mode timer.
- src/riscv-csr.h        : Functions and macros to access RISC-V CSRs (Generated file)
- src/riscv-interrupts.h : List of RISC-V machine mode interrupts.

Build Files:

- src/CMakeLists.txt       : CMake build file.
- Makefile                 : Makefile to configure and run CMake, and upload and debug on SiFive HiFive revb board.

Other Files:

- src/linker.lds           : Linker script for SiFive HiFive revb board (from the metal environment).


NOTE:
   CSR include file sourced from:
      wget https://raw.githubusercontent.com/five-embeddev/riscv-csr-access/master/include/riscv-csr.h
