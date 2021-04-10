/*
   Baremetal main program with timer interrupt.
   SPDX-License-Identifier: Unlicense

   https://five-embeddev.com/

   Tested with sifive-hifive-revb, but should not have any
   dependencies to any particular implementation.
   
*/

#include <cstdint>
#include <chrono>

// RISC-V CSR definitions and access classes
// Download: wget https://raw.githubusercontent.com/five-embeddev/riscv-csr-access/master/include/riscv-csr.hpp
#include "riscv-csr.hpp"
#include "riscv-interrupts.hpp"
#include "timer.hpp"

// Machine mode interrupt service routine
static void irq_entry(void) __attribute__ ((interrupt ("machine")));

// Timer driver 
static driver::timer<> mtimer;

// Global to hold current timestamp
static volatile uint64_t timestamp{0};

int main(void) {
    // Global interrupt disable
    riscv::csrs.mstatus.mie.clr();

    // Setup timer for 1 second interval
    timestamp = mtimer.get_time<driver::timer<>::timer_ticks>().count();
    mtimer.set_time_cmp(std::chrono::seconds{1});
    // Setup the IRQ handler entry point
    riscv::csrs.mtvec.write( reinterpret_cast<std::uintptr_t>(irq_entry));

    // Timer interrupt enable
    riscv::csrs.mie.mti.set();
    // Global interrupt enable
    riscv::csrs.mstatus.mie.set();

    // Busy loop
    do {
        __asm__ volatile ("wfi");  
    } while (true);
    
    return 0;
}

#pragma GCC push_options
// Force the alignment for mtvec.BASE. A 'C' extension program could be aligned to to bytes.
#pragma GCC optimize ("align-functions=4")
static void irq_entry(void)  {
    auto this_cause = riscv::csrs.mcause.read();
    if (this_cause &  riscv::csr::mcause_data::interrupt::BIT_MASK) {
        this_cause &= 0xFF;
        // Known exceptions
        switch (this_cause) {
        case riscv::interrupts::mti :
            // Timer exception, keep up the one second tick.
            mtimer.set_time_cmp(std::chrono::seconds{1});
            timestamp = mtimer.get_time<driver::timer<>::timer_ticks>().count();
            break;
        }
    }
}
#pragma GCC pop_options

    
