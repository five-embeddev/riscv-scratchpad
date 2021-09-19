/*
   Simple machine mode timer driver for RISC-V standard timer.
   SPDX-License-Identifier: Unlicense

   (https://five-embeddev.com/) 

*/

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define RISCV_MTIMECMP_ADDR (0x2000000 + 0x4000)
#define RISCV_MTIME_ADDR    (0x2000000 + 0xBFF8)

#define MTIME_FREQ_HZ 32768

#define MTIMER_SECONDS_TO_CLOCKS(SEC)           \
    ((uint64_t)(((SEC)*(MTIME_FREQ_HZ))))

/** Set the raw time compare point in system timer clocks.
 * @param clock_offset Time relative to current mtime when 
 * @note The time range of the 64 bit timer is large enough not to consider a wrap around of mtime.
 * An interrupt will be generated at mtime + clock_offset.
 * See http://five-embeddev.com/riscv-isa-manual/latest/machine.html#machine-timer-registers-mtime-and-mtimecmp
 */
void mtimer_set_raw_time_cmp(uint64_t clock_offset);

/** Read the raw time of the system timer in system timer clocks
 */
uint64_t mtimer_get_raw_time(void);
            

#endif // #ifdef TIMER_H

