/*
   Baremetal main program with timer interrupt.
   SPDX-License-Identifier: Unlicense

   https://five-embeddev.com/

   Tested with sifive-hifive-revb, but should not have any
   dependencies to any particular implementation.
   
*/


// Makes use of GCC interrupt and weak reference/alias attributes
// https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes
// https://gcc.gnu.org/onlinedocs/gcc/RISC-V-Function-Attributes.html#RISC-V-Function-Attributes

// Vector table - not to be called.
void riscv_mtvec_table(void)  __attribute__ ((naked));
void riscv_stvec_table(void)  __attribute__ ((naked));
void riscv_utvec_table(void)  __attribute__ ((naked));

// Default "NOP" implementations
static void riscv_nop_machine(void)    __attribute__ ((interrupt ("machine")) );
static void riscv_nop_supervisor(void) __attribute__ ((interrupt ("supervisor")) );
static void riscv_nop_user(void)       __attribute__ ((interrupt ("user")) );

// Weak alias to the "NOP" implementations. If another function  
void riscv_mtvec_exception(void) __attribute__ ((interrupt ("machine")     , weak, alias("riscv_nop_machine") )); 
void riscv_mtvec_msi(void) __attribute__ ((interrupt ("machine")     , weak, alias("riscv_nop_machine") )); 
void riscv_mtvec_mti(void) __attribute__ ((interrupt ("machine")     , weak, alias("riscv_nop_machine") ));
void riscv_mtvec_mei(void) __attribute__ ((interrupt ("machine")     , weak, alias("riscv_nop_machine") ));
void riscv_mtvec_ssi(void) __attribute__ ((interrupt ("supervisor")  , weak, alias("riscv_nop_machine") ));
void riscv_mtvec_sti(void) __attribute__ ((interrupt ("supervisor")  , weak, alias("riscv_nop_machine") ));
void riscv_mtvec_sei(void) __attribute__ ((interrupt ("supervisor")  , weak, alias("riscv_nop_machine") ));

void riscv_stvec_exception(void) __attribute__ ((interrupt ("machine")     , weak, alias("riscv_nop_supervisor") )); 
void riscv_stvec_ssi(void) __attribute__ ((interrupt ("supervisor")  , weak, alias("riscv_nop_supervisor") ));
void riscv_stvec_sti(void) __attribute__ ((interrupt ("supervisor")  , weak, alias("riscv_nop_supervisor") ));
void riscv_stvec_sei(void) __attribute__ ((interrupt ("supervisor")  , weak, alias("riscv_nop_supervisor") ));

void riscv_utvec_usi(void) __attribute__ ((interrupt ("user")        , weak, alias("riscv_nop_user") ));
void riscv_utvec_uti(void) __attribute__ ((interrupt ("user")        , weak, alias("riscv_nop_user") ));
void riscv_utvec_uei(void) __attribute__ ((interrupt ("user")        , weak, alias("riscv_nop_user") ));

#pragma GCC push_options

// Ensure the vector table is aligned.
// The bottom 4 bits of MTVEC are ignored - so align to 16 bytes

// Vector table. Do not call!
// Possible entries defined by mcause table
// http://five-embeddev.com/riscv-isa-manual/latest/machine.html#sec:mcause
//
// When vectored interrupts are enabled, interrupt cause 0, which
// corresponds to user-mode software interrupts, are vectored to the
// same location as synchronous exceptions. This ambiguity does not
// arise in practice, since user-mode software interrupts are either
// disabled or delegated to user mode.
void riscv_mtvec_table(void) {
    __asm__ volatile (
        ".align 16;"
        ".org  0*4;"
        "jal   zero,riscv_mtvec_exception;"  /* 0  */   
        ".org  1*4;"
        "jal   zero,riscv_mtvec_ssi;"  /* 1  */   
        ".org 3*4;"
        "jal   zero,riscv_mtvec_msi;"  /* 3  */   
        ".org  5*4;"
        "jal   zero,riscv_mtvec_sti;"  /* 5  */   
        ".org  7*4;"
        "jal   zero,riscv_mtvec_mti;"  /* 7  */   
        ".org  9*4;"
        "jal   zero,riscv_mtvec_sei;"  /* 9  */   
        ".org  11*4;"
        "jal   zero,riscv_mtvec_mei;"  /* 11 */   
        : /* output: none */                    
        : /* input : immediate */               
        : /* clobbers: none */
        );
}
// Vector table. Do not call!
// See scause table for possible entries.
// http://five-embeddev.com/riscv-isa-manual/latest/supervisor.html#sec:scause
void riscv_stvec_table(void) {
    __asm__ volatile (                          
        ".align 16;"
        ".org  1*4;"
        "jal   zero,riscv_mtvec_ssi;"  /* 1  */   
        ".org  5*4;"
        "jal   zero,riscv_mtvec_sti;"  /* 5  */   
        ".org  9*4;"
        "jal   zero,riscv_mtvec_sei;"  /* 9  */   
        : /* output: none */                    
        : /* input : immediate */               
        : /* clobbers: none */
        );
}
// Vector table. Do not call!
void riscv_utvec_table(void) {
    __asm__ volatile (                          
        ".align 16;"
        ".org  0*4;"
        "jal   zero,riscv_utvec_usi;"  /* 0  */   
        ".org 4*4;"
        "jal   zero,riscv_utvec_uti;"  /* 4  */   
        ".org  8*4;"
        "jal   zero,riscv_utvec_uei;"  /* 8  */   
        : /* output: none */                    
        : /* input : immediate */               
        : /* clobbers: none */
        );
}

// Ensure all ISR functions are aligned.
#pragma GCC optimize ("align-functions=4")

static void riscv_nop_machine(void)  {
    // Nop machine mode interrupt.
}
static void riscv_nop_supervisor(void) {
    // Nop supervisor mode interrupt.
}
static void riscv_nop_user(void) {
    // Nop user mode interrupt.
}

#pragma GCC pop_options

