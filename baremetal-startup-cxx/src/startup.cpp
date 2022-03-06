/*
   Simple C++ startup routine to setup CRT
   SPDX-License-Identifier: Unlicense

   (https://five-embeddev.com/ | http://www.shincbm.com/) 

*/

#include <algorithm>
#include <cstdint>

// Generic C function pointer.
typedef void (*function_t)();

// These symbols are defined by the linker script.
// See linker.lds
extern "C" std::uintptr_t metal_segment_bss_target_start;
extern "C" std::uintptr_t metal_segment_bss_target_end;
extern "C" std::uintptr_t metal_segment_data_source_start;
extern "C" std::uintptr_t metal_segment_data_target_start;
extern "C" std::uintptr_t metal_segment_data_target_end;
extern "C" std::uintptr_t metal_segment_itim_source_start;
extern "C" std::uintptr_t metal_segment_itim_target_start;
extern "C" std::uintptr_t metal_segment_itim_target_end;

extern "C" function_t __init_array_start[];
extern "C" function_t __init_array_end[];

// This function will be placed by the linker script according to the section
extern "C" void _enter(void)  __attribute__ ((naked, section(".text.metal.init.enter")));

// Define the symbols with "C" naming as they are used by the assembler
extern "C" void _start(void) __attribute__ ((noreturn));
extern "C" void _Exit(int exit_code) __attribute__ ((noreturn));

// Standard entry point, no arguments.
extern int main(void);

// The linker script will place this in the reset entry point.
// It will be 'called' with no stack or C runtime configuration.
// NOTE - this only supports a single hart.
// tp will not be initialized
void _enter(void)   {
    // Setup SP and GP
    // The locations are defined in the linker script
    __asm__ volatile  (
        ".option push;"
        // The 'norelax' option is critical here.
        // Without 'norelax' the global pointer will
        // be loaded relative to the global pointer!
        ".option norelax;"
        "la    gp, __global_pointer$;"
        ".option pop;"
        "la    sp, _sp;"
        "jal   zero, _start;"
        :  /* output: none %0 */
        : /* input: none */
        : /* clobbers: none */); 
    // This point will not be executed, _start() will be called with no return.
}

// At this point we have a stack and global poiner, but no access to global variables.
void _start(void) {

    // Init memory regions
    // Clear the .bss section (global variables with no initial values)
    std::fill(&metal_segment_bss_target_start, // cppcheck-suppress mismatchingContainers
              &metal_segment_bss_target_end,
              0U);
    // Initialize the .data section (global variables with initial values)
    std::copy(&metal_segment_data_source_start, // cppcheck-suppress mismatchingContainers
              &metal_segment_data_source_start + (&metal_segment_data_target_end-&metal_segment_data_target_start),
              &metal_segment_data_target_start);
    // Initialize the .itim section (code moved from flash to SRAM to improve performance)
    std::copy(&metal_segment_itim_source_start, // cppcheck-suppress mismatchingContainers
              &metal_segment_itim_source_start + (&metal_segment_itim_target_end - &metal_segment_itim_target_start),
              &metal_segment_itim_target_start);

    // Call constructors
    std::for_each( __init_array_start,
                   __init_array_end, 
                   [](const function_t pf) {pf();});
    // Jump to main
    auto rc = main();
    // Don't expect to return, if so busy loop in the exit function.
    _Exit(rc);
}

// This should never be called. Busy loop with the CPU in idle state.
void _Exit(int exit_code) { 
    // Halt
    while (true) {
        __asm__ volatile ("wfi");
    }
}

