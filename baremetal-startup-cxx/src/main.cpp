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
static void irq_entry(void) noexcept __attribute__ ((interrupt ("machine")));

// Timer driver 
static driver::timer<> mtimer;

// Global to hold current timestamp
static volatile uint64_t timestamp{0};

// Class to perform constructor/destructor before main()
template<uint32_t V>
class TestStaticConstructDestruct {
public:
    TestStaticConstructDestruct(unsigned int &v) 
        : _v(v) {
        _v |= V;
    }
    ~TestStaticConstructDestruct() {
        _v &= ~V;
    }
private:
    unsigned int &_v;
};

// Values for tracing and observing initialization
static int global_value_with_init = 42;
static uint32_t global_u32_value_with_init{0xa1a2a3a4ul};
static uint64_t global_u64_value_with_init{0xb1b2b3b4b5b6b7b8ull};
static float    global_f32_value_with_init{3.14};
static double   global_f64_value_with_init{1.44};
static uint16_t global_u16_value_with_init{0x1234};
static uint8_t  global_u8a_value_with_init{0x42};
static uint8_t  global_u8b_value_with_init{0x43};
static uint8_t  global_u8c_value_with_init{0x44};
static uint8_t  global_u8d_value_with_init{0x45};
static volatile bool global_bool_keep_running{true};

// Values to observe contructor/destructor changes
static unsigned int global_value1_with_constructor = 1;
static unsigned int global_value2_with_constructor = 2;

// Constructors and destructors
static TestStaticConstructDestruct<0x200> constructor_destructor_1(global_value1_with_constructor);
static TestStaticConstructDestruct<0x200> constructor_destructor_2(global_value2_with_constructor);
static TestStaticConstructDestruct<0x100000> constructor_destructor_3{global_value2_with_constructor};
static TestStaticConstructDestruct<0x100000> constructor_destructor_4{global_value1_with_constructor};

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
    riscv::csrs.mstatus.mie.clr();
    
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

    
