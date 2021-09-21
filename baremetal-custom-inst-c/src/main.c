/*
   Baremetal main program with timer interrupt.
   SPDX-License-Identifier: Unlicense

   https://five-embeddev.com/

   Tested with sifive-hifive-revb, but should not have any
   dependencies to any particular implementation.
   
*/

// RISC-V CSR definitions and access classes
#include "riscv-csr.h"
#include "riscv-interrupts.h"

// Machine mode interrupt service routine
static void irq_entry(void) __attribute__ ((interrupt ("machine")));

enum {
    CUSTOM_MAC_Rn_Rm
}

int main(void) {
    // Global interrupt disable
    CSR_CLR_BITS_IMM_MSTATUS(MSTATUS_MIE_BIT_MASK);
    
    // Setup the IRQ handler entry point
    csr_write_mtvec((uint_xlen_t) irq_entry);

    // Global interrupt enable 
    CSR_SET_BITS_IMM_MSTATUS(MSTATUS_MIE_BIT_MASK);

    
    


    // Busy loop
    do {
        __asm__ volatile ("wfi");  
    } while (1);
    
    return 0;

}

#pragma GCC push_options
// Force the alignment for mtvec.BASE. A 'C' extension program could be aligned to to bytes.
#pragma GCC optimize ("align-functions=4")
static void irq_entry(void)  {
    uint_xlen_t this_cause = csr_read_mcause();
    if ((this_cause & MCAUSE_INTERRUPT_BIT_MASK) == 0) {
    }
}
#pragma GCC pop_options


