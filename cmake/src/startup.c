// Dummy startup file. 
// NOTE - this is just an example to complete linking.
// THIS FILE IS NOT FUNCTIONAL.

// This function will be placed by the linker script according to the section
// Raw function 'called' by the CPU with no runtime.
void _enter(void)  __attribute__ ((naked, section(".text.metal.init.enter")));

// Entry and exit points as C functions.
void _start(void) __attribute__ ((noreturn));
void _exit(int) __attribute__ ((noreturn));

// Standard entry point, no arguments.
extern int main(void);

// This function will be placed by the linker script according to the section
void _enter(void) {
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
    // NOTE - just an example to link - this is completely NON-FUNCTIONAL
    // This function does not setup a functional C runtime!
    // Jump to main
    // Don't expect to return, if so busy loop in the exit function.
    _exit(main());
}

// This should never be called. Busy loop with the CPU in idle state.
void _exit(int exit_code) {
    // Halt
    while (1) {
        __asm__ volatile ("wfi");
    }
}
