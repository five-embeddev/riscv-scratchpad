/*
   Baremetal main program with timer interrupt.
   SPDX-License-Identifier: Unlicense

   https://five-embeddev.com/

   Tested with sifive-hifive-revb, but should not have any
   dependencies to any particular implementation.
   
*/

#include <stdint.h>
#include <stdbool.h>

// RISC-V CSR definitions and access classes
#include "riscv-csr.h"
#include "riscv-interrupts.h"
#include "timer.h"

// Machine mode interrupt service routine
static void irq_entry(void) __attribute__ ((interrupt ("machine")));

// Constructors
static void setup_global2(void) __attribute__ ((constructor(102)));
static void setup_global1(void) __attribute__ ((constructor(101)));

// Destructors
static void destroy_global1(void) __attribute__ ((destructor(101)));
static void destroy_global2(void) __attribute__ ((destructor(102)));

// Global to hold current timestamp
static volatile uint64_t timestamp = 0;

// Values for tracing and observing initialization
static int global_value_with_init = 42;
static uint32_t global_u32_value_with_init = 0xa1a2a3a4ul;
static uint64_t global_u64_value_with_init = 0xb1b2b3b4b5b6b7b8ull;
static float    global_f32_value_with_init = 3.14;
static double   global_f64_value_with_init = 1.44;
static uint16_t global_u16_value_with_init = 0x1234;
static uint8_t  global_u8a_value_with_init = 0x42;
static uint8_t  global_u8b_value_with_init = 0x43;
static uint8_t  global_u8c_value_with_init = 0x44;
static uint8_t  global_u8d_value_with_init = 0x45;
static volatile bool global_bool_keep_running = true;

// Values to observe contructor/destructor changes
static unsigned int global_value1_with_constructor = 1;
static unsigned int global_value2_with_constructor = 2;

int main(void) {
    // Global interrupt disable
    csr_clr_bits_mstatus(MSTATUS_MIE_BIT_MASK);
    csr_write_mie(0);

    // Setup timer for 1 second interval
    timestamp = mtimer_get_raw_time();
    mtimer_set_raw_time_cmp(MTIMER_SECONDS_TO_CLOCKS(1));
    
    // Setup the IRQ handler entry point
    csr_write_mtvec((uint_xlen_t) irq_entry);

    // Enable MIE.MTI
    csr_set_bits_mie(MIE_MTI_BIT_MASK);

    // Global interrupt enable 
    csr_set_bits_mstatus(MSTATUS_MIE_BIT_MASK);

    // Increment for testing
    global_u8c_value_with_init++;
    global_u32_value_with_init++;
    global_u64_value_with_init++;
    global_f32_value_with_init++;
    global_u8b_value_with_init++;
    global_f64_value_with_init++;
    global_u8d_value_with_init++;
    global_u16_value_with_init++;
    global_u8a_value_with_init++;

    // Busy loop
    do {
        __asm__ volatile ("wfi");  
        global_value_with_init++;
    } while (global_bool_keep_running);

    // Global interrupt disable
    csr_clr_bits_mstatus(MSTATUS_MIE_BIT_MASK);
    
    return 0;
}

#pragma GCC push_options
// Force the alignment for mtvec.BASE. A 'C' extension program could be aligned to to bytes.
#pragma GCC optimize ("align-functions=4")
static void irq_entry(void)  {
    uint_xlen_t this_cause = csr_read_mcause();
    if (this_cause &  MCAUSE_INTERRUPT_BIT_MASK) {
        this_cause &= 0xFF;
        // Known exceptions
        switch (this_cause) {
        case RISCV_INT_MASK_MTI :
            // Timer exception, keep up the one second tick.
            mtimer_set_raw_time_cmp(MTIMER_SECONDS_TO_CLOCKS(1));
            timestamp = mtimer_get_raw_time();
            break;
        }
    }
}
#pragma GCC pop_options


void setup_global2(void) {
    global_value1_with_constructor |= 0x200;
    global_value2_with_constructor |= 0x200;
}

void setup_global1(void) {
    global_value1_with_constructor |= 0x100000;
    global_value2_with_constructor |= 0x100000;
}

void destroy_global2(void) {
    global_value1_with_constructor &= ~0x200;
    global_value2_with_constructor &= ~0x200;
}

void destroy_global1(void) {
    global_value1_with_constructor &= ~0x100000;
    global_value2_with_constructor &= ~0x100000;
}
