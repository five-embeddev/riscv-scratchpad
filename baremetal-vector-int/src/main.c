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

// Global to hold current timestamp, written in MTI handler.
static volatile uint64_t timestamp = 0;
// Expect this to increment one time per second - inside exception handler, after each return of MTI handler.
static volatile uint64_t ecall_count = 0;

#define RISCV_MTVEC_MODE_VECTORED 1

int main(void) {
    // Global interrupt disable
    csr_clr_bits_mstatus(MSTATUS_MIE_BIT_MASK);
    csr_write_mie(0);
    
    // Setup the IRQ handler entry point, set the mode to vectored
    csr_write_mtvec((uint_xlen_t) riscv_mtvec_table | RISCV_MTVEC_MODE_VECTORED);

    // Enable MIE.MTI
    csr_set_bits_mie(MIE_MTI_BIT_MASK);

    // Global interrupt enable 
    csr_set_bits_mstatus(MSTATUS_MIE_BIT_MASK);

    // Setup timer for 1 second interval
    timestamp = mtimer_get_raw_time();
    mtimer_set_raw_time_cmp(MTIMER_SECONDS_TO_CLOCKS(1));
    
    // Busy loop
    do {
        // Wait for timer interrupt
        __asm__ volatile ("wfi");
        // Try a synchronous exception.
        __asm__ volatile ("ecall");
    } while (1);
    
    // Will not reach here
    return 0;
}

#pragma GCC push_options
// Force the alignment for mtvec.BASE. A 'C' extension program could be aligned to to bytes.
#pragma GCC optimize ("align-functions=4")
// The 'riscv_mtvec_mti' function is added to the vector table by the vector_table.c
void riscv_mtvec_mti(void)  {
    // Timer exception, re-program the timer for a one second tick.
    mtimer_set_raw_time_cmp(MTIMER_SECONDS_TO_CLOCKS(1));
    timestamp = mtimer_get_raw_time();
}
// The 'riscv_mtvec_exception' function is added to the vector table by the vector_table.c
// This function looks at the cause of the exception, if it is an 'ecall' instruction then increment a global counter.
void riscv_mtvec_exception(void)  {
    uint_xlen_t this_cause = csr_read_mcause();
    uint_xlen_t this_pc    = csr_read_mepc();
    //uint_xlen_t this_value = csr_read_mtval();
    switch (this_cause) {
        case RISCV_EXCP_ENVIRONMENT_CALL_FROM_M_MODE:
            ecall_count++;
            // Make sure the return address is the instruction AFTER ecall
            csr_write_mepc(this_pc+4);
            break;
    }
}
#pragma GCC pop_options


