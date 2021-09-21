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
#include "timer.h"

#include "vector_table.h"

// Machine mode interrupt service routine

// Global to hold current timestamp
static volatile uint64_t timestamp = 0;

int main(void) {
    // Global interrupt disable
    CSR_CLR_BITS_IMM_MSTATUS(MSTATUS_MIE_BIT_MASK);
    
    // Setup the IRQ handler entry point
    csr_write_mtvec((uint_xlen_t) riscv_mtvec_table | 0x1);

    // Global interrupt enable 
    CSR_SET_BITS_IMM_MSTATUS(MSTATUS_MIE_BIT_MASK);

    // Setup timer for 1 second interval
    timestamp = mtimer_get_raw_time();
    mtimer_set_raw_time_cmp(MTIMER_SECONDS_TO_CLOCKS(1));
    
    // Busy loop
    do {
        __asm__ volatile ("wfi");  
    } while (1);
    
    return 0;

}

#pragma GCC push_options
// Force the alignment for mtvec.BASE. A 'C' extension program could be aligned to to bytes.
#pragma GCC optimize ("align-functions=4")
void riscv_mtvec_mti(void)  {
    // Timer exception, keep up the one second tick.
    mtimer_set_raw_time_cmp(MTIMER_SECONDS_TO_CLOCKS(1));
    timestamp = mtimer_get_raw_time();
}
void riscv_mtvec_exception(void)  {
}
#pragma GCC pop_options


